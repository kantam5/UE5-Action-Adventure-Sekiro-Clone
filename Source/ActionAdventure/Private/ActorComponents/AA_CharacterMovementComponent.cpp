// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/AA_CharacterMovementComponent.h"

#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/AA_CharacterBase.h"
#include "AbilitySystem/AttributeSets/AA_AttributeSetBase.h"

UAA_CharacterMovementComponent::UAA_CharacterMovementComponent()
	: MovementDirectionType(EMovementDirectionType::OrientToMovement)
	, RequestToSpeedUp(false)
	, RequestToSpeedDown(false)
{
}

void UAA_CharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	HandleMovementDirection();

	if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		AbilityComponent->RegisterGameplayTagEvent(
			FGameplayTag::RequestGameplayTag(FName("Movement.Enforced.Strafe"), EGameplayTagEventType::NewOrRemoved))
			.AddUObject(this, &UAA_CharacterMovementComponent::OnEnforcedStrafeTagChanged);

		AbilityComponent->RegisterGameplayTagEvent(
			FGameplayTag::RequestGameplayTag(FName("State.Debuff.Groggy"), EGameplayTagEventType::NewOrRemoved))
			.AddUObject(this, &UAA_CharacterMovementComponent::OnTagChangedUpdateRotationRate);
		AbilityComponent->RegisterGameplayTagEvent(
			FGameplayTag::RequestGameplayTag(FName("State.BlockInput"), EGameplayTagEventType::NewOrRemoved))
			.AddUObject(this, &UAA_CharacterMovementComponent::OnTagChangedUpdateRotationRate);
		AbilityComponent->RegisterGameplayTagEvent(
			FGameplayTag::RequestGameplayTag(FName("State.InAir.Hook"), EGameplayTagEventType::NewOrRemoved))
			.AddUObject(this, &UAA_CharacterMovementComponent::OnTagChangedUpdateRotationRate);

	}
}

void UAA_CharacterMovementComponent::OnEnforcedStrafeTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount)
	{
		SetMovementDirectionType(EMovementDirectionType::Strafe);
	}
	else
	{
		SetMovementDirectionType(EMovementDirectionType::OrientToMovement);
	}
}

void UAA_CharacterMovementComponent::OnTagChangedUpdateRotationRate(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount)
	{
		RotationRate = FRotator::ZeroRotator;
	}
	else
	{
		RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	}
}

void UAA_CharacterMovementComponent::HandleMovementDirection()
{
	switch (MovementDirectionType)
	{
	case EMovementDirectionType::Strafe:
		bUseControllerDesiredRotation = true;
		bOrientRotationToMovement = false;
		CharacterOwner->bUseControllerRotationYaw = true;
		break;
	default:
		bUseControllerDesiredRotation = false;
		bOrientRotationToMovement = true;
		CharacterOwner->bUseControllerRotationYaw = false;
		break;
	}
}

float UAA_CharacterMovementComponent::GetMaxSpeed() const
{
	AAA_CharacterBase* Owner = Cast<AAA_CharacterBase>(GetOwner());
	if (!Owner)
	{
		return Super::GetMaxSpeed();
	}

	if (!Owner->IsAlive())
	{
		return 0.0f;
	}

	if (Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Groggy")))
		|| Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.BlockInput")))
		|| Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.InAir.Hook"))))
	{
		return 0.0f;
	}

	if (!Owner->GetIsArmed())
	{
		return Owner->GetAttributeSet()->GetMaxMovementSpeed() * 0.5f;
	}

	if (RequestToSpeedUp)
	{
		return Owner->GetAttributeSet()->GetMaxMovementSpeed() * 1.6f;
	}

	if (RequestToSpeedDown)
	{
		return Owner->GetAttributeSet()->GetMaxMovementSpeed() * 0.5f;
	}

	return Owner->GetAttributeSet()->GetMaxMovementSpeed();
}

void UAA_CharacterMovementComponent::StartSpeedUp()
{
	RequestToSpeedUp = true;
}

void UAA_CharacterMovementComponent::StopSpeedUp()
{
	RequestToSpeedUp = false;
}

void UAA_CharacterMovementComponent::StartSpeedDown()
{
	RequestToSpeedDown = true;
}

void UAA_CharacterMovementComponent::StopSpeedDown()
{
	RequestToSpeedDown = false;
}

void UAA_CharacterMovementComponent::SetMovementDirectionType(EMovementDirectionType InMovementDirectionType)
{
	MovementDirectionType = InMovementDirectionType;

	HandleMovementDirection();
}

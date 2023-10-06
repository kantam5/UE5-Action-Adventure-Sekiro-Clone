// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_PlungingDeathBlow.h"

#include "Characters/PlayerCharacter.h"
#include "Characters/EnemyCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/AA_AIController.h"
#include "DataAssets/CharacterAnimDataAsset.h"

bool UGA_PlungingDeathBlow::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	OwnerCharacter = Cast<APlayerCharacter>(GetOwningActorFromActorInfo());
	if (!OwnerCharacter.Get())
	{
		return false;
	}

	TargetCharacter = Cast<AEnemyCharacterBase>(OwnerCharacter->GetPlungeEnemy());
	if (!TargetCharacter.Get())
	{
		return false;
	}

	AAA_AIController* AIController = Cast<AAA_AIController>(TargetCharacter->GetController());
	if (AIController)
	{
		if (AIController->GetTarget() == OwnerCharacter)
		{
			return false;
		}
	}

	if (!OwnerCharacter->GetCharacterMovement()->IsFalling())
	{
		return false;
	}

	return true;
}

void UGA_PlungingDeathBlow::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	TargetCharacter->GetCharacterMovement()->DisableMovement();

	UAnimMontage* MontageToPlay = OwnerCharacter->GetCharacterData().CharacterAnimDataAsset->CharacterAnimationData.PlungeMontage;
	
	FGameplayTagContainer GameplayTagContainer = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.DeathBlow.Plunging")));
	MontageTask = UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, GameplayTagContainer);
	MontageTask->OnBlendOut.AddDynamic(this, &UGA_PlungingDeathBlow::OnCompleted);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_PlungingDeathBlow::OnCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_PlungingDeathBlow::OnCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_PlungingDeathBlow::OnCancelled);
	MontageTask->EventReceived.AddDynamic(this, &UGA_PlungingDeathBlow::EventReceived);
	MontageTask->ReadyForActivation();

	PlungeToTarget = UAT_PlungeToTarget::PlungeToTarget(this, NAME_None, TargetCharacter.Get());
	PlungeToTarget->ReadyForActivation();

	OwnerCharacter->BeginPlungeCollisionOverlap();
}

void UGA_PlungingDeathBlow::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_PlungingDeathBlow::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_PlungingDeathBlow::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::ApplyEffect(EventTag, EventData);
}


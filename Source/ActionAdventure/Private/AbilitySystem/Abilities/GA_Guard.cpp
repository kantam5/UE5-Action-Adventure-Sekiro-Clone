// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_Guard.h"

#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "ActorComponents/AA_CharacterMovementComponent.h"
#include "Characters/AA_CharacterBase.h"

void UGA_Guard::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	if (Character)
	{
		UAA_CharacterMovementComponent* MovementComponent = Cast<UAA_CharacterMovementComponent>(Character->GetCharacterMovement());
		if (MovementComponent)
		{
			MovementComponent->StartSpeedDown();
		}
	}
}

void UGA_Guard::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	if (Character)
	{
		UAA_CharacterMovementComponent* MovementComponent = Cast<UAA_CharacterMovementComponent>(Character->GetCharacterMovement());
		if (MovementComponent)
		{
			MovementComponent->StopSpeedDown();
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

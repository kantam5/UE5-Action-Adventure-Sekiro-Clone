// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_Sprint.h"

#include "GameFramework/Character.h"
#include "ActorComponents/AA_CharacterMovementComponent.h"

void UGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	if (Character)
	{
		UAA_CharacterMovementComponent* MovementComponent = Cast<UAA_CharacterMovementComponent>(Character->GetCharacterMovement());
		if (MovementComponent)
		{
			MovementComponent->StartSpeedUp();
		}
	}
	
}

void UGA_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	if (Character)
	{
		UAA_CharacterMovementComponent* MovementComponent = Cast<UAA_CharacterMovementComponent>(Character->GetCharacterMovement());
		if (MovementComponent)
		{
			MovementComponent->StopSpeedUp();
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

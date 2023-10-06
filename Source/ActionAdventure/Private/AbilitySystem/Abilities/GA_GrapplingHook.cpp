// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_GrapplingHook.h"

#include "Characters/PlayerCharacter.h"
#include "DataAssets/CharacterAnimDataAsset.h"
#include "ActorComponents/GrapplingHookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AA_CharacterTypes.h"

bool UGA_GrapplingHook::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
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

	if (OwnerCharacter->CanHookThrow())
	{
		return true;
	}

	return false;
}

void UGA_GrapplingHook::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	OwnerCharacter->GetCharacterMovement()->GravityScale = 0.3f;

	UAnimMontage* MontageToPlay = OwnerCharacter->GetHookMontage();
	OwnerCharacter->GetGrapplingHookComponent()->PlayInputAnimation();

	FGameplayTagContainer GameplayTagContainer = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName("Event.Montage.Hook")));
	MontageTask = UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, GameplayTagContainer);
	MontageTask->OnBlendOut.AddDynamic(this, &UGA_GrapplingHook::OnCompleted);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_GrapplingHook::OnCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_GrapplingHook::OnCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_GrapplingHook::OnCancelled);
	MontageTask->EventReceived.AddDynamic(this, &UGA_GrapplingHook::EventReceived);
	MontageTask->ReadyForActivation();
}

void UGA_GrapplingHook::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

	if (OwnerCharacter.Get())
	{
		OwnerCharacter->GetCharacterMovement()->GravityScale = 1.2f;
	}
}

void UGA_GrapplingHook::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

	if (OwnerCharacter.Get())
	{
		OwnerCharacter->GetCharacterMovement()->GravityScale = 1.2f;
	}
}

void UGA_GrapplingHook::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::ApplyEffect(EventTag, EventData);
}

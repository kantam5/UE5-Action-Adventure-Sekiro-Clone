// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_PotionBase.h"

#include "Player/AA_PlayerController.h"
#include "DataAssets/AA_AssetManager.h"
#include "Characters/PlayerCharacter.h"

void UGA_PotionBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	OwnerCharacter = Cast<APlayerCharacter>(GetOwningActorFromActorInfo());
	if (OwnerCharacter.Get())
	{
		OwnerCharacter->SetUpperBodyMontage(true);
	}

	FGameplayTagContainer GameplayTagContainer = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("Event.Montage")));
	MontageTask = UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, GameplayTagContainer);
	MontageTask->OnBlendOut.AddDynamic(this, &UGA_PotionBase::OnCompleted);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_PotionBase::OnCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_PotionBase::OnCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_PotionBase::OnCancelled);
	MontageTask->EventReceived.AddDynamic(this, &UGA_PotionBase::EventReceived);
	MontageTask->ReadyForActivation();
}

void UGA_PotionBase::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

	if (OwnerCharacter.Get())
	{
		OwnerCharacter->SetUpperBodyMontage(false);
	}
}

void UGA_PotionBase::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

	if (OwnerCharacter.Get())
	{
		OwnerCharacter->SetUpperBodyMontage(false);
	}
}

void UGA_PotionBase::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::ApplyEffect(EventTag, EventData);

	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetActorInfo().PlayerController);
	if (PotionItem)
	{
		PlayerController->RemoveInventoryItem(PotionItem, 1);
	}

	if (OwnerCharacter.Get())
	{
		OwnerCharacter->SetUpperBodyMontage(false);
	}
}

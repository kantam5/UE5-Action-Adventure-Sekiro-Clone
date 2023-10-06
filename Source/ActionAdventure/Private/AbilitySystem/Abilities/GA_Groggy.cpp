// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_Groggy.h"

#include "Characters/AA_CharacterBase.h"
#include "AnimInstances/AA_AnimInstance.h"
#include "AA_CharacterTypes.h"
#include "DataAssets/CharacterAnimDataAsset.h"
#include "AbilitySystemComponent.h"

UGA_Groggy::UGA_Groggy()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;

	/*AAA_CharacterBase* Character = Cast<AAA_CharacterBase>(GetOwningActorFromActorInfo());
	if (Character)
	{
		MontageToPlay = Character->GetCharacterData().CharacterAnimDataAsset->CharacterAnimationData.GroggyMontage;
	}*/
}

void UGA_Groggy::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AAA_CharacterBase* Character = Cast<AAA_CharacterBase>(GetOwningActorFromActorInfo());
	if (Character)
	{
		MontageToPlay = Character->GetCharacterData().CharacterAnimDataAsset->CharacterAnimationData.GroggyMontage;
		Character->GetAniminstance()->SetIsGroggying(true);
		Character->GetAbilitySystemComponent()->CancelAbilities();
		Character->StopAnimMontage();
	}

	FGameplayTagContainer GameplayTagContainer = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.Groggy")));
	MontageTask = UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, GameplayTagContainer);
	MontageTask->OnBlendOut.AddDynamic(this, &UGA_Groggy::OnCompleted);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_Groggy::OnCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_Groggy::OnCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_Groggy::OnCancelled);
	MontageTask->EventReceived.AddDynamic(this, &UGA_Groggy::EventReceived);
	MontageTask->ReadyForActivation();
}

void UGA_Groggy::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCancelled"));
	AAA_CharacterBase* Character = Cast<AAA_CharacterBase>(GetOwningActorFromActorInfo());
	if (Character)
	{
		Character->GetAniminstance()->SetIsGroggying(false);
	}

	// EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_Groggy::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCompleted"));
	AAA_CharacterBase* Character = Cast<AAA_CharacterBase>(GetOwningActorFromActorInfo());
	if (Character)
	{
		Character->GetAniminstance()->SetIsGroggying(false);
	}

	// EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_Groggy::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::ApplyEffect(EventTag, EventData);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_Melee.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Character.h"
#include "Characters/AA_CharacterBase.h"
#include "Characters/PlayerCharacter.h"
#include "Animation/AnimInstance.h"
#include "AbilitySystemComponent.h"
#include "Actors/WeaponActor.h"

UGA_Melee::UGA_Melee()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

bool UGA_Melee::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	OwnerCharacter = CastChecked<AAA_CharacterBase>(ActorInfo->AvatarActor.Get());
	if (!OwnerCharacter.Get())
	{
		return false;
	}

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		return !AnimInstance->Montage_IsPlaying(MontageToPlay);
	}

	return false;
}

void UGA_Melee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayTagContainer GameplayTagContainer = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("Event.Montage")));
	MontageTask = UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, GameplayTagContainer);
	MontageTask->OnBlendOut.AddDynamic(this, &UGA_Melee::OnCompleted);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_Melee::OnCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_Melee::OnCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_Melee::OnCancelled);
	MontageTask->EventReceived.AddDynamic(this, &UGA_Melee::EventReceived);
	MontageTask->ReadyForActivation();
}

void UGA_Melee::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

	AWeaponActor* Weapon = OwnerCharacter.Get() ? OwnerCharacter->GetWeapon() : nullptr;
	if (Weapon)
	{
		Weapon->EndWeaponAttack();
	}
}

void UGA_Melee::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

	AWeaponActor* Weapon = OwnerCharacter.Get() ? OwnerCharacter->GetWeapon() : nullptr;
	if (Weapon)
	{
		Weapon->EndWeaponAttack();
	}
}

void UGA_Melee::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::ApplyEffect(EventTag, EventData);

}


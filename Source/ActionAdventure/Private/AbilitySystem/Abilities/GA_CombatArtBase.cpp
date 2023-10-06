// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_CombatArtBase.h"

#include "Characters/AA_CharacterBase.h"
#include "Actors/WeaponActor.h"
#include "MotionWarpingComponent.h"

void UGA_CombatArtBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayTagContainer GameplayTagContainer = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("Event.Montage")));
	MontageTask = UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, GameplayTagContainer);
	MontageTask->OnBlendOut.AddDynamic(this, &UGA_CombatArtBase::OnCompleted);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_CombatArtBase::OnCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_CombatArtBase::OnCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_CombatArtBase::OnCancelled);
	MontageTask->EventReceived.AddDynamic(this, &UGA_CombatArtBase::EventReceived);
	MontageTask->ReadyForActivation();
}

void UGA_CombatArtBase::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

	AAA_CharacterBase* OwnerCharacter = Cast<AAA_CharacterBase>(GetOwningActorFromActorInfo());
	AWeaponActor* Weapon = OwnerCharacter ? OwnerCharacter->GetWeapon() : nullptr;
	if (Weapon)
	{
		Weapon->EndWideRangeWeaponAttack();
	}
}

void UGA_CombatArtBase::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

	AAA_CharacterBase* OwnerCharacter = Cast<AAA_CharacterBase>(GetOwningActorFromActorInfo());
	AWeaponActor* Weapon = OwnerCharacter ? OwnerCharacter->GetWeapon() : nullptr;
	if (Weapon)
	{
		Weapon->EndWideRangeWeaponAttack();
	}
}

void UGA_CombatArtBase::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::ApplyEffect(EventTag, EventData);

	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.Shared.SetRushLocation")))
	{
		if (bNeedMotionWarping)
		{
			AAA_CharacterBase* OwnerCharacter = Cast<AAA_CharacterBase>(GetOwningActorFromActorInfo());
			if (OwnerCharacter->GetTargetCharacter() != nullptr)
			{
				FVector TargetLocation = OwnerCharacter->GetTargetCharacter()->GetActorLocation();
				FVector OwnerLocation = OwnerCharacter->GetActorLocation();
				if (FVector::Dist(TargetLocation, OwnerLocation) > RushDistance)
				{
					OwnerCharacter->GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocation(
						TEXT("Rush"), OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * RushDistance);
				}
				else
				{
					OwnerCharacter->GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocation(
						TEXT("Rush"), TargetLocation - OwnerCharacter->GetActorForwardVector() * 150.f);
				}
			}
			else
			{
				OwnerCharacter->GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocation(
					TEXT("Rush"), OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * RushDistance);
			}
		}
	}
}

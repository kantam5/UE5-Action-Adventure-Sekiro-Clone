// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_BackstabDeathBlow.h"

#include "Characters/EnemyCharacterBase.h"
#include "AnimInstances/AA_AnimInstance.h"
#include "DataAssets/CharacterAnimDataAsset.h"
#include "Characters/PlayerCharacter.h"
#include "MotionWarpingComponent.h"
#include "Kismet/KismetMathLibrary.h"

UGA_BackstabDeathBlow::UGA_BackstabDeathBlow()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

bool UGA_BackstabDeathBlow::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
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

	if (OwnerCharacter->CanBackstabDeathBlow())
	{
		return true;
	}

	return false;

}

void UGA_BackstabDeathBlow::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	TargetCharacter = OwnerCharacter->GetBackstabEnemy();
	if (!TargetCharacter.Get())
	{
		return;
	}

	UMotionWarpingComponent* MotionWarpingComp = OwnerCharacter->GetMotionWarpingComponent();
	if (MotionWarpingComp)
	{
		FRotator EndRot = UKismetMathLibrary::FindLookAtRotation(
			OwnerCharacter->GetActorLocation(), TargetCharacter->GetActorLocation());
		EndRot.Pitch = 0.f;
		EndRot.Roll = 0.f;

		MotionWarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(
			FName("JumpToTarget"), OwnerCharacter->GetActorLocation(), EndRot);
	}

	FGameplayTag MontageTag = FGameplayTag::RequestGameplayTag(FName("Event.Montage.DeathBlow.Backstab"));
	UAnimMontage* MontageToPlay = *OwnerCharacter->GetCharacterData().CharacterAnimDataAsset->CharacterAnimationData.DeathBlowMontageMap.Find(MontageTag);

	FGameplayTagContainer GameplayTagContainer = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.DeathBlow.Default")));
	MontageTask = UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, GameplayTagContainer);
	MontageTask->OnBlendOut.AddDynamic(this, &UGA_BackstabDeathBlow::OnCompleted);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_BackstabDeathBlow::OnCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_BackstabDeathBlow::OnCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_BackstabDeathBlow::OnCancelled);
	MontageTask->EventReceived.AddDynamic(this, &UGA_BackstabDeathBlow::EventReceived);
	MontageTask->ReadyForActivation();

	if (TargetCharacter.Get())
	{
		TargetCharacter->HandleDeathBlow(MontageTag, OwnerCharacter.Get());
	}
}

void UGA_BackstabDeathBlow::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if (TargetCharacter.Get())
	{
		TargetCharacter->HandleEndDeathBlow();
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_BackstabDeathBlow::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if (TargetCharacter.Get())
	{
		TargetCharacter->HandleEndDeathBlow();
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_BackstabDeathBlow::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::ApplyEffect(EventTag, EventData);
}

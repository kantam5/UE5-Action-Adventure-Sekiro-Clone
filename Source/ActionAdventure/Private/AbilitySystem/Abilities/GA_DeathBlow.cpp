// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_DeathBlow.h"

#include "Characters/PlayerCharacter.h"
#include "Characters/EnemyCharacterBase.h"
#include "AnimInstances/AA_AnimInstance.h"
#include "DataAssets/CharacterAnimDataAsset.h"
#include "AA_CharacterTypes.h"
#include "MotionWarpingComponent.h"
#include "Actors/WeaponActor.h"

UGA_DeathBlow::UGA_DeathBlow()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

bool UGA_DeathBlow::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
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

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter.Get());
	Params.AddIgnoredActor(Cast<AActor>(OwnerCharacter->GetWeapon()));
	FCollisionResponseParams ResponseParams;
	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		OwnerCharacter->GetActorLocation(),
		OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * 350.f,
		FQuat::Identity,
		ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(50.0f),
		Params,
		ResponseParams);

	if (bResult && HitResults.Num() > 0)
	{
		TargetCharacter = Cast<AEnemyCharacterBase>(HitResults[0].GetActor());
	}

	if (!TargetCharacter.Get())
	{
		return false;
	}

	if (TargetCharacter->GetAniminstance())
	{
		if (TargetCharacter->GetAniminstance()->GetIsGroggying())
		{
			return true;
		}
	}

	return false;
}

void UGA_DeathBlow::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayTag MontageTag = FGameplayTag::RequestGameplayTag(FName("Event.Montage.DeathBlow.Default"));
	UAnimMontage* MontageToPlay = *OwnerCharacter->GetCharacterData().CharacterAnimDataAsset->CharacterAnimationData.DeathBlowMontageMap.Find(MontageTag);

	FGameplayTagContainer GameplayTagContainer = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.DeathBlow.Default")));
	MontageTask = UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, GameplayTagContainer);
	MontageTask->OnBlendOut.AddDynamic(this, &UGA_DeathBlow::OnCompleted);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_DeathBlow::OnCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_DeathBlow::OnCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_DeathBlow::OnCancelled);
	MontageTask->EventReceived.AddDynamic(this, &UGA_DeathBlow::EventReceived);
	MontageTask->ReadyForActivation();

	if (TargetCharacter.Get())
	{
		TargetCharacter->HandleDeathBlow(MontageTag, OwnerCharacter.Get());
	}
}

void UGA_DeathBlow::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if (TargetCharacter.Get())
	{
		TargetCharacter->HandleEndDeathBlow();
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_DeathBlow::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if (TargetCharacter.Get())
	{
		TargetCharacter->HandleEndDeathBlow();
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_DeathBlow::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::ApplyEffect(EventTag, EventData);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_FinisherDeathBlow.h"

#include "Characters/PlayerCharacter.h"
#include "Characters/BossCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AA_PlayerController.h"

bool UGA_FinisherDeathBlow::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
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

	if (OwnerCharacter->GetCanFinish())
	{
		return true;
	}

	return false;
}

void UGA_FinisherDeathBlow::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController->IsValidLowLevel())
	{
		PlayerController->SetBossUIVisibility(false);
	}

	FGameplayTagContainer GameplayTagContainer = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.DeathBlow.Finisher")));
	MontageTask = UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, GameplayTagContainer);
	MontageTask->OnBlendOut.AddDynamic(this, &UGA_FinisherDeathBlow::OnCompleted);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_FinisherDeathBlow::OnCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_FinisherDeathBlow::OnCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_FinisherDeathBlow::OnCancelled);
	MontageTask->EventReceived.AddDynamic(this, &UGA_FinisherDeathBlow::EventReceived);
	MontageTask->ReadyForActivation();

	if (OwnerCharacter->GetBoss())
	{
		OwnerCharacter->CancelLockOn();
		OwnerCharacter->GetBoss()->PlayFinishedMontage();
	}
}

void UGA_FinisherDeathBlow::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

	if (OwnerCharacter.Get())
	{
		OwnerCharacter->EndFinisherDeathBlow();
	}
}

void UGA_FinisherDeathBlow::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

	if (OwnerCharacter.Get())
	{
		OwnerCharacter->EndFinisherDeathBlow();
	}
}

void UGA_FinisherDeathBlow::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::ApplyEffect(EventTag, EventData);
}

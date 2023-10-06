// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AA_GameplayAbility.h"
#include "AbilitySystem/AbilityTasks/AT_PlayMontageAndWaitForEvent.h"
#include "GA_BackstabDeathBlow.generated.h"

class UAT_PlayMontageAndWaitForEvent;
class APlayerCharacter;
class AEnemyCharacterBase;

UCLASS()
class ACTIONADVENTURE_API UGA_BackstabDeathBlow : public UAA_GameplayAbility
{
	GENERATED_BODY()

public:
	mutable TWeakObjectPtr<APlayerCharacter> OwnerCharacter;

	TWeakObjectPtr<AEnemyCharacterBase> TargetCharacter;

	UAT_PlayMontageAndWaitForEvent* MontageTask;

	UGA_BackstabDeathBlow();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

protected:
	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
};

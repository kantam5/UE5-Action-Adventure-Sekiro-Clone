// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AA_GameplayAbility.h"
#include "AbilitySystem/AbilityTasks/AT_PlayMontageAndWaitForEvent.h"
#include "AbilitySystem/AbilityTasks/AT_PlungeToTarget.h"
#include "GA_PlungingDeathBlow.generated.h"

class APlayerCharacter;
class AEnemyCharacterBase;

UCLASS()
class ACTIONADVENTURE_API UGA_PlungingDeathBlow : public UAA_GameplayAbility
{
	GENERATED_BODY()

public:
	mutable TWeakObjectPtr<APlayerCharacter> OwnerCharacter;

	mutable TWeakObjectPtr<AEnemyCharacterBase> TargetCharacter;

	UAT_PlayMontageAndWaitForEvent* MontageTask;
	
	UAT_PlungeToTarget* PlungeToTarget;
	
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

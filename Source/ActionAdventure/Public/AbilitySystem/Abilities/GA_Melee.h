// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AA_GameplayAbility.h"
#include "AbilitySystem/AbilityTasks/AT_PlayMontageAndWaitForEvent.h"
#include "GA_Melee.generated.h"

class AAA_CharacterBase;
class UAbilityTask_PlayMontageAndWait;
class UAT_PlayMontageAndWaitForEvent;

UCLASS()
class ACTIONADVENTURE_API UGA_Melee : public UAA_GameplayAbility
{
	GENERATED_BODY()

public:
	mutable TWeakObjectPtr<AAA_CharacterBase> OwnerCharacter;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* MontageToPlay;

	UPROPERTY()
	UAT_PlayMontageAndWaitForEvent* MontageTask;

	UGA_Melee();

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

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AA_GameplayAbility.h"
#include "AbilitySystem/AbilityTasks/AT_PlayMontageAndWaitForEvent.h"
#include "GA_CombatArtBase.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UGA_CombatArtBase : public UAA_GameplayAbility
{
	GENERATED_BODY()
	
public:
	UAT_PlayMontageAndWaitForEvent* MontageTask;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* MontageToPlay;

	UPROPERTY(EditDefaultsOnly)
	bool bNeedMotionWarping;

	UPROPERTY(EditDefaultsOnly)
	float RushDistance;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

protected:
	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
};

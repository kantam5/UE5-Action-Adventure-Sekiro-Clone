// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AA_GameplayAbility.h"
#include "AbilitySystem/AbilityTasks/AT_PlayMontageAndWaitForEvent.h"
#include "GA_PotionBase.generated.h"

class APlayerCharacter;
class UAA_PotionItem;
class UAA_Item;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UGA_PotionBase : public UAA_GameplayAbility
{
	GENERATED_BODY()
	
public:
	TWeakObjectPtr<APlayerCharacter> OwnerCharacter;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* MontageToPlay;

	UPROPERTY(EditDefaultsOnly)
	UAA_Item* PotionItem;

	UAT_PlayMontageAndWaitForEvent* MontageTask;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);

};

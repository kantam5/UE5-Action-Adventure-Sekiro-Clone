// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AA_GameplayAbility.h"
#include "AbilitySystem/AbilityTasks/AT_PlayMontageAndWaitForEvent.h"
#include "GA_FireProjectile.generated.h"

class AAA_Projectile;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UGA_FireProjectile : public UAA_GameplayAbility
{
	GENERATED_BODY()
	
public:
	UAT_PlayMontageAndWaitForEvent* MontageTask;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* MontageToPlay;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<AAA_Projectile> ProjectileClass;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

protected:
	UPROPERTY(EditAnywhere)
	float Range;

	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AA_GameplayAbility.h"
#include "GA_LockOn.generated.h"

class APlayerCharacter;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UGA_LockOn : public UAA_GameplayAbility
{
	GENERATED_BODY()

public:
	mutable TWeakObjectPtr<APlayerCharacter> OwnerCharacter;

	mutable FHitResult LockOnHitResult;

	UPROPERTY(EditDefaultsOnly)
	float LockOnDistance = 2000.f;

	UPROPERTY(EditDefaultsOnly)
	float LockOnSphereRadius = 200.f;

	UGA_LockOn();
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);
};

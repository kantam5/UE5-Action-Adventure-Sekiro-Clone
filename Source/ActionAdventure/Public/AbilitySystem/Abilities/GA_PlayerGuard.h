// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GA_Guard.h"
#include "GA_PlayerGuard.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UGA_PlayerGuard : public UGA_Guard
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> DeflectEffect;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};

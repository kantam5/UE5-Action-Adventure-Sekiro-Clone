// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AA_AbilitySystemComponent.generated.h"

class UAA_GameplayAbility;

UCLASS()
class ACTIONADVENTURE_API UAA_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, TArray<UAA_GameplayAbility*>& ActiveAbilities);

	
};

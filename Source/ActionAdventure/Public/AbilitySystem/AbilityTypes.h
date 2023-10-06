// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityTypes.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FGameplayEffectContainer
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effects")
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffectClasses;

};
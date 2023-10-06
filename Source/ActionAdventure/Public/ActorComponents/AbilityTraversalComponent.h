// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilityTraversalComponent.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONADVENTURE_API UAbilityTraversalComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	bool TryJumpTraversal(UAbilitySystemComponent* ASC);
		
	bool TryShiftTraversal(UAbilitySystemComponent* ASC);

	bool TryNormalAttackTraversal(UAbilitySystemComponent* ASC);

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> TraversalJumpAbilitiesOrdered;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> TraversalShiftAbilitiesOrdered;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> TraversalNormalAttackAbilitiesOrdered;

};

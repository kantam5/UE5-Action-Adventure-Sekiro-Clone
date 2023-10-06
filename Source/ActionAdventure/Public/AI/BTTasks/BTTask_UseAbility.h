// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_UseAbility.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UBTTask_UseAbility : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_UseAbility();

protected:
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer GameplayAbilityTags;

	bool bIsActivating;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UFUNCTION()
	void OnAbilityEnded(UGameplayAbility* GameplayAbility);

};

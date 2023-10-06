// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ModifyMovementSpeed.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UBTTask_ModifyMovementSpeed : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_ModifyMovementSpeed(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere)
	bool RequestToSpeedUp;

	UPROPERTY(EditAnywhere)
	bool RequestToSpeedDown;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

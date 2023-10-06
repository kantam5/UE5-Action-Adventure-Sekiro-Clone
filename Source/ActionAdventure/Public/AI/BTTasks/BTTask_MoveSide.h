// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveSide.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UBTTask_MoveSide : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MoveSide(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Category = Move, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MoveTime;
	UPROPERTY(Category = Move, EditAnywhere, meta = (UIMin = 0, ClampMin = 0))
	float RandomDeviation;

	float RemainingWaitTime;
	float AccumulatedDeltaTime;

	float RemainigChangeDirectionTime;
	float AccumulatedMoveDirectionDeltaTime;

	bool MoveDirection;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_StopInvestigating.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UBTTask_StopInvestigating : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_StopInvestigating(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

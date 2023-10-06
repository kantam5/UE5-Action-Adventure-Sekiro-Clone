// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetLocationAround.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UBTTask_SetLocationAround : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float ToPlayerDistance;
	UPROPERTY(EditAnywhere)
	float ToPlayerRandomDeviation;

	UPROPERTY(EditAnywhere)
	float ToSideDistance;
	UPROPERTY(EditAnywhere)
	float ToSideRandomDeviation;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

};

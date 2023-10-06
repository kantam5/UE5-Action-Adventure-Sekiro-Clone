// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SetMoveLocation.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UBTService_SetMoveLocation : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_SetMoveLocation();

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};

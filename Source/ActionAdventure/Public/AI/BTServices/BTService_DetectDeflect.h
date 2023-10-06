// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_DetectDeflect.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UBTService_DetectDeflect : public UBTService
{
	GENERATED_BODY()

	UBTService_DetectDeflect();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};

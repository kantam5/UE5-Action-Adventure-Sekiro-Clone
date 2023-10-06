// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_RandomLoop.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UBTDecorator_RandomLoop : public UBTDecorator
{
	GENERATED_BODY()

//protected:
//	UPROPERTY(EditAnywhere, Category = "RandomLoop")
//	int32 BaseLoops;
//
//	UPROPERTY(EditAnywhere, Category = "RandomLoop")
//	int32 RandomDeviation;
//
//	int32  NumLoops;

	/** number of executions */
	/*UPROPERTY(Category = Decorator, EditAnywhere, meta = (EditCondition = "!bInfiniteLoop", ClampMin = "1"))
	int32 NumLoops;*/

	/** infinite loop */
	/*UPROPERTY(Category = Decorator, EditAnywhere)
	bool bInfiniteLoop;*/

	/** timeout (when looping infinitely, when we finish a loop we will check whether we have spent this time looping, if we have we will stop looping). A negative value means loop forever. */
	/*UPROPERTY(Category = Decorator, EditAnywhere, meta = (EditCondition = "bInfiniteLoop"))
	float InfiniteLoopTimeoutTime;*/

//
//	/*virtual uint16 GetInstanceMemorySize() const override;
//	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;
//	virtual FString GetStaticDescription() const override;*/
//
//protected:
//	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
//
//	virtual void OnNodeActivation(FBehaviorTreeSearchData& SearchData) override;
};

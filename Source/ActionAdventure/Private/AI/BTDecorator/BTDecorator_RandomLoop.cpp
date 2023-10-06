// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator/BTDecorator_RandomLoop.h"

#include "BehaviorTree/Composites/BTComposite_SimpleParallel.h"
#include "BehaviorTree/Decorators/BTDecorator_Loop.h"

//UBTDecorator_RandomLoop::UBTDecorator_RandomLoop(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//	/*NodeName = "Loop";
//	NumLoops = FMath::RandRange(FMath::Max(0, BaseLoops - RandomDeviation), (BaseLoops + RandomDeviation));
//	INIT_DECORATOR_NODE_NOTIFY_FLAGS();
//
//	bAllowAbortNone = false;
//	bAllowAbortLowerPri = false;
//	bAllowAbortChildNodes = false;*/
//}

////UBTDecorator_RandomLoop::UBTDecorator_RandomLoop(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
////{
////	NodeName = "Loop";
////	NumLoops = 3;
////	InfiniteLoopTimeoutTime = -1.f;
////	INIT_DECORATOR_NODE_NOTIFY_FLAGS();
////
////	bAllowAbortNone = false;
////	bAllowAbortLowerPri = false;
////	bAllowAbortChildNodes = false;
////}
//
////uint16 UBTDecorator_RandomLoop::GetInstanceMemorySize() const
////{
////	return sizeof(FBTLoopDecoratorMemory);
////}
////
////void UBTDecorator_RandomLoop::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
////{
////	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);
////
////	if (!bInfiniteLoop)
////	{
////		FBTLoopDecoratorMemory* DecoratorMemory = (FBTLoopDecoratorMemory*)NodeMemory;
////		Values.Add(FString::Printf(TEXT("loops remaining: %d"), DecoratorMemory->RemainingExecutions));
////	}
////	else if (InfiniteLoopTimeoutTime > 0.f)
////	{
////		FBTLoopDecoratorMemory* DecoratorMemory = (FBTLoopDecoratorMemory*)NodeMemory;
////
////		const float TimeRemaining = FMath::Max(InfiniteLoopTimeoutTime - (GetWorld()->GetTimeSeconds() - DecoratorMemory->TimeStarted), 0.f);
////		Values.Add(FString::Printf(TEXT("time remaining: %s"), *FString::SanitizeFloat(TimeRemaining)));
////	}
////}
////
////FString UBTDecorator_RandomLoop::GetStaticDescription() const
////{
////	if (bInfiniteLoop)
////	{
////		if (InfiniteLoopTimeoutTime < 0.f)
////		{
////			return FString::Printf(TEXT("%s: infinite"), *Super::GetStaticDescription());
////		}
////		else
////		{
////			return FString::Printf(TEXT("%s: loop for %s seconds"), *Super::GetStaticDescription(), *FString::SanitizeFloat(InfiniteLoopTimeoutTime));
////		}
////	}
////	else
////	{
////		return FString::Printf(TEXT("%s: %d loops"), *Super::GetStaticDescription(), NumLoops);
////	}
////}
//
//void UBTDecorator_RandomLoop::OnNodeActivation(FBehaviorTreeSearchData& SearchData)
//{
//	//FBTLoopDecoratorMemory* DecoratorMemory = GetNodeMemory<FBTLoopDecoratorMemory>(SearchData);
//	//FBTCompositeMemory* ParentMemory = GetParentNode()->GetNodeMemory<FBTCompositeMemory>(SearchData);
//	//const bool bIsSpecialNode = GetParentNode()->IsA(UBTComposite_SimpleParallel::StaticClass());
//
//	//if ((bIsSpecialNode && ParentMemory->CurrentChild == BTSpecialChild::NotInitialized) ||
//	//	(!bIsSpecialNode && ParentMemory->CurrentChild != ChildIndex))
//	//{
//	//	// initialize counter if it's first activation
//	//	DecoratorMemory->RemainingExecutions = NumLoops;
//	//	DecoratorMemory->TimeStarted = GetWorld()->GetTimeSeconds();
//	//}
//
//	//bool bShouldLoop = false;
//	//if (bInfiniteLoop)
//	//{
//	//	// protect from truly infinite loop within single search
//	//	if (SearchData.SearchId != DecoratorMemory->SearchId)
//	//	{
//	//		if ((InfiniteLoopTimeoutTime < 0.f) || ((DecoratorMemory->TimeStarted + InfiniteLoopTimeoutTime) > GetWorld()->GetTimeSeconds()))
//	//		{
//	//			bShouldLoop = true;
//	//		}
//	//	}
//
//	//	DecoratorMemory->SearchId = SearchData.SearchId;
//	//}
//	//else
//	//{
//	//	if (DecoratorMemory->RemainingExecutions > 0)
//	//	{
//	//		DecoratorMemory->RemainingExecutions--;
//	//	}
//	//	bShouldLoop = DecoratorMemory->RemainingExecutions > 0;
//	//}
//
//
//	//// set child selection overrides
//	//if (bShouldLoop)
//	//{
//	//	GetParentNode()->SetChildOverride(SearchData, ChildIndex);
//	//}
//}

//bool UBTDecorator_RandomLoop::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
//{
//	return Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
//}
//
//void UBTDecorator_RandomLoop::OnNodeActivation(FBehaviorTreeSearchData& SearchData)
//{
//	//FBTLoopDecoratorMemory* DecoratorMemory = GetNodeMemory<FBTLoopDecoratorMemory>(SearchData);
//	//FBTCompositeMemory* ParentMemory = GetParentNode()->GetNodeMemory<FBTCompositeMemory>(SearchData);
//	//const bool bIsSpecialNode = GetParentNode()->IsA(UBTComposite_SimpleParallel::StaticClass());
//
//	//if ((bIsSpecialNode && ParentMemory->CurrentChild == BTSpecialChild::NotInitialized) ||
//	//	(!bIsSpecialNode && ParentMemory->CurrentChild != ChildIndex))
//	//{
//	//	// initialize counter if it's first activation
//	//	DecoratorMemory->RemainingExecutions = NumLoops;
//	//	DecoratorMemory->TimeStarted = GetWorld()->GetTimeSeconds();
//	//}
//
//	//bool bShouldLoop = false;
//	//if (DecoratorMemory->RemainingExecutions > 0)
//	//{
//	//	DecoratorMemory->RemainingExecutions--;
//	//}
//	//bShouldLoop = DecoratorMemory->RemainingExecutions > 0;
//
//	//// set child selection overrides
//	//if (bShouldLoop)
//	//{
//	//	GetParentNode()->SetChildOverride(SearchData, ChildIndex);
//	//}
//}

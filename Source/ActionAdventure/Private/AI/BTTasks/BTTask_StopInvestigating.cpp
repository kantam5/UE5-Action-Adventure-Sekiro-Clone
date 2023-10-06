// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_StopInvestigating.h"

#include "AI/AA_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_StopInvestigating::UBTTask_StopInvestigating(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Stop Investigating");
}

EBTNodeResult::Type UBTTask_StopInvestigating::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAA_AIController* const OwnerAI = Cast<AAA_AIController>(OwnerComp.GetAIOwner());
	if (OwnerAI)
	{
		OwnerAI->GetBlackboardComponent()->SetValueAsBool(AAA_AIController::InvestigatingByFingerWhistleKey, false);
		OwnerAI->GetBlackboardComponent()->SetValueAsBool(AAA_AIController::InvestigatingKey, false);
		OwnerAI->GetBlackboardComponent()->SetValueAsVector(AAA_AIController::NoiseLocationKey, FVector::ZeroVector);
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

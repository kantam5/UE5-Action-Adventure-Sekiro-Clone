// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_StopCounter.h"

#include "AI/AA_AIController.h"
#include "Characters/EnemyCharacterBase.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_StopCounter::UBTTask_StopCounter(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Stop Counter");
}

EBTNodeResult::Type UBTTask_StopCounter::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAA_AIController* const OwnerAI = Cast<AAA_AIController>(OwnerComp.GetAIOwner());

	AEnemyCharacterBase* const OwnerCharacter = Cast<AEnemyCharacterBase>(OwnerAI->GetPawn());
	if (OwnerCharacter)
	{
		OwnerCharacter->SetCounter(false);
	}

	if (OwnerAI)
	{
		OwnerAI->GetBlackboardComponent()->SetValueAsBool(AAA_AIController::DeflectKey, false);
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

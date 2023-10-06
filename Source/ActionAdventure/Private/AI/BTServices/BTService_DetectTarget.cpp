// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTServices/BTService_DetectTarget.h"

#include "AIController.h"
#include "Characters/EnemyCharacterBase.h"
#include "AI/AA_AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_DetectTarget::UBTService_DetectTarget()
{
	NodeName = TEXT("DetectTarget");
}

void UBTService_DetectTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr)
	{
		return;
	}

	AEnemyCharacterBase* Owner = Cast<AEnemyCharacterBase>(ControllingPawn);
	AAA_AIController* OwnerAI = Cast<AAA_AIController>(UAIBlueprintHelperLibrary::GetAIController(Owner));
	if (OwnerAI->GetTarget() == nullptr)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAA_AIController::TargetKey, nullptr);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAA_AIController::TargetKey, OwnerAI->GetTarget());
	}

	return;
}

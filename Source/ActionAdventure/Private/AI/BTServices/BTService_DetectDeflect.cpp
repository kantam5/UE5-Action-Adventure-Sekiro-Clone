// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTServices/BTService_DetectDeflect.h"

#include "AI/AA_AIController.h"
#include "Characters/EnemyCharacterBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_DetectDeflect::UBTService_DetectDeflect()
{
	NodeName = TEXT("DetectDeflect");
}

void UBTService_DetectDeflect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr)
	{
		return;
	}

	AEnemyCharacterBase* Owner = Cast<AEnemyCharacterBase>(ControllingPawn);
	if (Owner->GetCounter())
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(AAA_AIController::DeflectKey, true);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(AAA_AIController::DeflectKey, false);
	}

	return;
}

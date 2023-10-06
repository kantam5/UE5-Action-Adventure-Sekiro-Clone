// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTServices/BTService_SetMoveLocation.h"

#include "AI/AA_AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/MinionCharacter.h"

UBTService_SetMoveLocation::UBTService_SetMoveLocation()
{
	NodeName = "SetMoveLocation";

	bNotifyBecomeRelevant = true;
	
}

void UBTService_SetMoveLocation::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr)
	{
		return;
	}

	AMinionCharacter* Owner = Cast<AMinionCharacter>(ControllingPawn);
	if (Owner)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AAA_AIController::MoveLocationKey, Owner->GetNextMoveLocation());
	}
}

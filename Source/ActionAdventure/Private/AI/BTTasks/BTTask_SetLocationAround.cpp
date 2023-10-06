// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_SetLocationAround.h"

#include "Characters/EnemyCharacterBase.h"	
#include "AI/AA_AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_SetLocationAround::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemyCharacterBase* OwnerCharacter = Cast<AEnemyCharacterBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (!OwnerCharacter)
	{
		return EBTNodeResult::Failed;
	}

	AAA_AIController* Controller = Cast<AAA_AIController>(OwnerComp.GetAIOwner());
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	AAA_CharacterBase* TargetCharacter = Cast<AAA_CharacterBase>(Controller->GetTarget());
	if (!TargetCharacter)
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(OwnerCharacter->GetWorld());
	if (!NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	const float PlayerDistance = FMath::FRandRange(FMath::Max(0.f, ToPlayerDistance - ToPlayerRandomDeviation), (ToPlayerDistance + ToPlayerRandomDeviation));
	const float SideDistance = FMath::FRandRange(FMath::Max(0.f, ToSideDistance - ToSideRandomDeviation), (ToSideDistance + ToSideRandomDeviation));
	if (FMath::RandBool())
	{
		const FVector Origin = TargetCharacter->GetActorLocation() + TargetCharacter->GetActorForwardVector() * PlayerDistance;
		const FVector NextPos = Origin + TargetCharacter->GetActorRightVector() * SideDistance;

		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AAA_AIController::MoveLocationKey, NextPos);

		return EBTNodeResult::Succeeded;
	}
	else
	{
		const FVector Origin = TargetCharacter->GetActorLocation() + TargetCharacter->GetActorForwardVector() * PlayerDistance;
		const FVector NextPos = Origin - TargetCharacter->GetActorRightVector() * SideDistance;

		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AAA_AIController::MoveLocationKey, NextPos);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

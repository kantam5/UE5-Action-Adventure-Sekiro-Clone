// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_MoveSide.h"

#include "Characters/EnemyCharacterBase.h"
#include "AI/AA_AIController.h"

UBTTask_MoveSide::UBTTask_MoveSide(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MoveTime(5.f)
	, RandomDeviation(3.f)
	, RemainingWaitTime(0.f)
	, AccumulatedDeltaTime(0.f)
	, RemainigChangeDirectionTime(0.f)
	, AccumulatedMoveDirectionDeltaTime(0.f)
	, MoveDirection(false)
{
	NodeName = "Move Side";
	bTickIntervals = true;
	bNotifyTick = true;
	MoveTime = 5.0f;
}

EBTNodeResult::Type UBTTask_MoveSide::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	MoveDirection = FMath::RandBool();
	RemainingWaitTime = FMath::FRandRange(FMath::Max(0.0f, MoveTime - RandomDeviation), (MoveTime + RandomDeviation));
	RemainigChangeDirectionTime = FMath::FRandRange(1.0f, 2.0f);

	return EBTNodeResult::InProgress;
}

void UBTTask_MoveSide::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AccumulatedDeltaTime += DeltaSeconds;
	AccumulatedMoveDirectionDeltaTime += DeltaSeconds;

	if (AccumulatedDeltaTime >= RemainingWaitTime)
	{
		AccumulatedDeltaTime = 0.0f;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (AccumulatedMoveDirectionDeltaTime >= RemainigChangeDirectionTime)
	{
		AccumulatedMoveDirectionDeltaTime = 0.0f;
		RemainigChangeDirectionTime = FMath::FRandRange(1.0f, 2.0f);

		MoveDirection = !MoveDirection;
	}

	AEnemyCharacterBase* OwnerCharacter = Cast<AEnemyCharacterBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (OwnerCharacter)
	{
		if (MoveDirection)
		{
			OwnerCharacter->AddMovementInput(OwnerCharacter->GetActorRightVector());
		}
		else
		{
			OwnerCharacter->AddMovementInput(-(OwnerCharacter->GetActorRightVector()));
		}
	}
	ensure(GetSpecialNodeMemory<FBTTaskMemory>(NodeMemory)->NextTickRemainingTime <= 0.f);

}

// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_TurnTowardNoise.h"

#include "AI/AA_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_TurnTowardNoise::UBTTask_TurnTowardNoise(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Turn Toward Noise");
}

EBTNodeResult::Type UBTTask_TurnTowardNoise::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAA_AIController* const OwnerAI = Cast<AAA_AIController>(OwnerComp.GetAIOwner());
	if (!OwnerAI)
	{
		return EBTNodeResult::Failed;
	}

	FVector NoiseLocation = OwnerAI->GetBlackboardComponent()->GetValueAsVector(AAA_AIController::NoiseLocationKey);

	FRotator CurrentControllerRotation = OwnerAI->GetControlRotation();
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(OwnerComp.GetOwner()->GetActorLocation(), NoiseLocation);
	FRotator NewRotation = FRotator(CurrentControllerRotation.Pitch, LookAtRotation.Yaw, CurrentControllerRotation.Roll);

	OwnerAI->GetPawn()->SetActorRotation(FMath::RInterpTo(CurrentControllerRotation,
		NewRotation, GetWorld()->GetDeltaSeconds(), 360.f));

	return EBTNodeResult::Succeeded;
}

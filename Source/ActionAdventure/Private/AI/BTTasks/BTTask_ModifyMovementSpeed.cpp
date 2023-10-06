// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_ModifyMovementSpeed.h"

#include "Characters/EnemyCharacterBase.h"
#include "AIController.h"
#include "ActorComponents/AA_CharacterMovementComponent.h"

UBTTask_ModifyMovementSpeed::UBTTask_ModifyMovementSpeed(const FObjectInitializer& ObjectInitializer)
	: RequestToSpeedUp(false)
	, RequestToSpeedDown(false)
{
	NodeName = "ModifyMovementSpeed";
}

EBTNodeResult::Type UBTTask_ModifyMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* const OwnerAI = OwnerComp.GetAIOwner();

	AEnemyCharacterBase* const OwnerCharacter = Cast<AEnemyCharacterBase>(OwnerAI->GetPawn());
	if (OwnerCharacter)
	{
		UAA_CharacterMovementComponent* CharacterMovement = Cast<UAA_CharacterMovementComponent>(OwnerCharacter->GetCharacterMovement());
		if (CharacterMovement)
		{
			if (RequestToSpeedUp)
			{
				CharacterMovement->StartSpeedUp();
			}
			else
			{
				CharacterMovement->StopSpeedUp();
			}

			if (RequestToSpeedDown)
			{
				CharacterMovement->StartSpeedDown();
			}
			else
			{
				CharacterMovement->StopSpeedDown();
			}
		}
	}

	return EBTNodeResult::Succeeded;
}

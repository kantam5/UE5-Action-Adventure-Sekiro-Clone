// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_CharacterMovement.h"

#include "Characters/PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "ActorComponents/GrapplingHookComponent.h"

UANS_CharacterMovement::UANS_CharacterMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UANS_CharacterMovement::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (OwnerCharacter->IsValidLowLevel())
	{
		StartLoc = OwnerCharacter->GetActorTransform().GetLocation();
		EndLoc = OwnerCharacter->GetGrapplingHookComponent()->GetLastFocusedPointLandLoc();
	}
}

void UANS_CharacterMovement::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (OwnerCharacter->IsValidLowLevel())
	{
		Location(FrameDeltaTime);
	}
}

void UANS_CharacterMovement::Location(float FrameDeltaTime)
{
	if (CurrentMovementTime < NotifyTotalTime)
	{
		OwnerCharacter->LerpHookCharacterMovement(StartLoc, EndLoc, CurrentMovementTime);

		CurrentMovementTime += FrameDeltaTime;
	}
}

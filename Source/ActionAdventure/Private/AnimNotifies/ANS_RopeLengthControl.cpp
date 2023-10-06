// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_RopeLengthControl.h"

#include "Characters/PlayerCharacter.h"

UANS_RopeLengthControl::UANS_RopeLengthControl(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UANS_RopeLengthControl::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void UANS_RopeLengthControl::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (OwnerCharacter->IsValidLowLevel())
	{
		UpdateRopeLength(FrameDeltaTime);
	}
}

void UANS_RopeLengthControl::UpdateRopeLength(float FrameDeltaTime)
{
	if (CurrentMovementTime < NotifyTotalTime)
	{
		OwnerCharacter->UpdateRopeLength(FrameDeltaTime);
		CurrentMovementTime += FrameDeltaTime;
	}
}

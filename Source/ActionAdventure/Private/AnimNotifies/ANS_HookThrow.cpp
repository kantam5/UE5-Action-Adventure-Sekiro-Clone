// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_HookThrow.h"

#include "Characters/PlayerCharacter.h"
#include "ActorComponents/GrapplingHookComponent.h"
#include "Kismet/KismetMathLibrary.h"

UANS_HookThrow::UANS_HookThrow(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UANS_HookThrow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (OwnerCharacter->IsValidLowLevel())
	{
		StartLoc = OwnerCharacter->HookTransform().GetLocation();
		EndLoc = OwnerCharacter->GetGrapplingHookComponent()->GetLastFocusedPointTrackerLoc();

		StartRot = OwnerCharacter->HookTransform().Rotator();
		EndRot = UKismetMathLibrary::FindLookAtRotation(StartLoc, EndLoc);
	}
}

void UANS_HookThrow::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (OwnerCharacter->IsValidLowLevel())
	{
		Rotation(FrameDeltaTime);
		Throw(FrameDeltaTime);
	}
}

void UANS_HookThrow::Rotation(float FrameDeltaTime)
{
	float Alpha = CurrentRotationTime / NotifyTotalTime;

	if (Alpha < 1.f)
	{
		OwnerCharacter->LerpHookRotation(StartRot, EndRot, Alpha);
		CurrentRotationTime += FrameDeltaTime * 2.f;
	}
}

void UANS_HookThrow::Throw(float FrameDeltaTime)
{
	if (CurrentMovementTime < NotifyTotalTime)
	{
		float NextFrameAlpha = CurrentMovementTime + FrameDeltaTime;

		OwnerCharacter->HookThrow(StartLoc, EndLoc, CurrentMovementTime, NextFrameAlpha);
		
		CurrentMovementTime += FrameDeltaTime;
	}
}

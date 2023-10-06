// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_HookPickUp.h"

#include "Characters/PlayerCharacter.h"

UANS_HookPickUp::UANS_HookPickUp(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UANS_HookPickUp::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (OwnerCharacter->IsValidLowLevel())
	{
		StartLoc = OwnerCharacter->HookTransform().GetLocation();
		EndLoc = OwnerCharacter->HookIdleSocketTransform().GetLocation();
	}
}

void UANS_HookPickUp::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (OwnerCharacter->IsValidLowLevel())
	{
		PickUp(FrameDeltaTime);
	}
}

void UANS_HookPickUp::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (OwnerCharacter->IsValidLowLevel())
	{
		OwnerCharacter->AttachHook();
	}
}

void UANS_HookPickUp::PickUp(float FrameDeltaTime)
{
	if (CurrentMovementTime < NotifyTotalTime)
	{
		OwnerCharacter->HookPickUp(StartLoc, OwnerCharacter->HookIdleSocketTransform().GetLocation(), CurrentMovementTime);

		CurrentMovementTime += FrameDeltaTime;
	}
}

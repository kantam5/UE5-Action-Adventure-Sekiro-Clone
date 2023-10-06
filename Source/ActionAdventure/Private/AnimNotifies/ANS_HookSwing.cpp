// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_HookSwing.h"

#include "Characters/PlayerCharacter.h"

UANS_HookSwing::UANS_HookSwing(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UANS_HookSwing::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void UANS_HookSwing::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (OwnerCharacter->IsValidLowLevel())
	{
		Transform(FrameDeltaTime);
	}
}

void UANS_HookSwing::Transform(float FrameDeltaTime)
{
	if (CurrentMovementTime < NotifyTotalTime)
	{
		OwnerCharacter->LerpHookTransform(
			OwnerCharacter->HookIdleSocketTransform(), 
			OwnerCharacter->HookSwingSocketTransform(),
			CurrentMovementTime);
		
		CurrentMovementTime += FrameDeltaTime;
	}
}

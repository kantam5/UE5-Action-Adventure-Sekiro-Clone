// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_TautenRope.h"

#include "Characters/PlayerCharacter.h"

UANS_TautenRope::UANS_TautenRope(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UANS_TautenRope::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (OwnerCharacter->IsValidLowLevel())
	{
		OwnerCharacter->SetRopeStiffness(true);
	}
}

void UANS_TautenRope::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (OwnerCharacter->IsValidLowLevel())
	{
		OwnerCharacter->SetRopeStiffness(false);
	}
}

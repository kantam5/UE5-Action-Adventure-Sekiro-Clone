// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/LerpNotifyStateBase.h"

#include "Characters/PlayerCharacter.h"

ULerpNotifyStateBase::ULerpNotifyStateBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	OwnerCharacter(nullptr),
	NotifyTotalTime(0.f),
	StartLoc(FVector::ZeroVector),
	EndLoc(FVector::ZeroVector),
	CurrentMovementTime(0.f),
	StartRot(FRotator::ZeroRotator),
	EndRot(FRotator::ZeroRotator),
	CurrentRotationTime(0.f)
{
}

void ULerpNotifyStateBase::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp->GetOwner()->IsValidLowLevel())
	{
		OwnerCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner());

		NotifyTotalTime = TotalDuration;
		CurrentMovementTime = 0.f;
		CurrentRotationTime = 0.f;
	}
}

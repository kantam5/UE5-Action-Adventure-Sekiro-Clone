// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifies/LerpNotifyStateBase.h"
#include "ANS_CharacterMovement.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UANS_CharacterMovement : public ULerpNotifyStateBase
{
	GENERATED_BODY()
	
public:
	UANS_CharacterMovement(const FObjectInitializer& ObjectInitializer);

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	virtual void Location(float FrameDeltaTime) override;
};

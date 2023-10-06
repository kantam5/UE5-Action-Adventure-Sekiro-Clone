// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifies/LerpNotifyStateBase.h"
#include "ANS_HookSwing.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UANS_HookSwing : public ULerpNotifyStateBase
{
	GENERATED_BODY()
	
public:
	UANS_HookSwing(const FObjectInitializer& ObjectInitializer);

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	virtual void Transform(float FrameDeltaTime) override;
};

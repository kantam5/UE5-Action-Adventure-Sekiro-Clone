// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifies/LerpNotifyStateBase.h"
#include "ANS_HookPickUp.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UANS_HookPickUp : public ULerpNotifyStateBase
{
	GENERATED_BODY()
	
public:
	UANS_HookPickUp(const FObjectInitializer& ObjectInitializer);

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	void PickUp(float FrameDeltaTime);

};

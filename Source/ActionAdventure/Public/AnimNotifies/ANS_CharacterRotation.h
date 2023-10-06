// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifies/LerpNotifyStateBase.h"
#include "ANS_CharacterRotation.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UANS_CharacterRotation : public ULerpNotifyStateBase
{
	GENERATED_BODY()
	
public:
	UANS_CharacterRotation(const FObjectInitializer& ObjectInitializer);

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	virtual void Rotation(float FrameDeltaTime) override;

};

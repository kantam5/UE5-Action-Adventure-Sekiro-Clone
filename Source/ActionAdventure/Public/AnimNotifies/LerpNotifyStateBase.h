// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "LerpNotifyStateBase.generated.h"

class APlayerCharacter;

UCLASS()
class ACTIONADVENTURE_API ULerpNotifyStateBase : public UAnimNotifyState
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	APlayerCharacter* OwnerCharacter;

	float NotifyTotalTime;

	FVector StartLoc;
	FVector EndLoc;
	float CurrentMovementTime;

	FRotator StartRot;
	FRotator EndRot;
	float CurrentRotationTime;

public:
	ULerpNotifyStateBase(const FObjectInitializer& ObjectInitializer);

	virtual void Transform(float FrameDeltaTime) PURE_VIRTUAL(ULerpNotifyStateBase::Transform(float FrameDeltaTime), ;);
	virtual void Location(float FrameDeltaTime) PURE_VIRTUAL(ULerpNotifyStateBase::Location(float FrameDeltaTime), ;);
	virtual void Rotation(float FrameDeltaTime) PURE_VIRTUAL(ULerpNotifyStateBase::Rotation(float FrameDeltaTime), ;);

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;


};

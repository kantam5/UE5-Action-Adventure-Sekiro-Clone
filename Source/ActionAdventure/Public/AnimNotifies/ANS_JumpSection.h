// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_JumpSection.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONADVENTURE_API UANS_JumpSection : public UAnimNotifyState
{
	GENERATED_BODY()

/*--------------------Variables---------------------*/
public:
	UPROPERTY(EditAnywhere)
	FName JumpSection;

/*--------------------Methods---------------------*/
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

};

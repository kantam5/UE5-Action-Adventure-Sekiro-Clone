// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AA_GameTypes.h"
#include "AnimNotify_FootStep.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UAnimNotify_FootStep : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere)
	EFoot Foot;
};

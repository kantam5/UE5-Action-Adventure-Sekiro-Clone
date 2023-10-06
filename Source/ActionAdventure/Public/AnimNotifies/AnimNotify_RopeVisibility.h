// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_RopeVisibility.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UAnimNotify_RopeVisibility : public UAnimNotify
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bRopeVisiblity;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};

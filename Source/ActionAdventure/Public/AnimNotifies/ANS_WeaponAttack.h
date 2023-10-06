// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "ANS_WeaponAttack.generated.h"

class AWeaponActor;

UCLASS()
class ACTIONADVENTURE_API UANS_WeaponAttack : public UAnimNotifyState
{
	GENERATED_BODY()

	/*--------------------Variables---------------------*/
	UPROPERTY(EditAnywhere)
	FGameplayTag EventTag;

/*--------------------Methods---------------------*/
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

};

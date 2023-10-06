// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AnimNotify_RandomPlaySound.h"

void UAnimNotify_RandomPlaySound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (Sounds.Num() > 0)
	{
		int32 PlayIndex = FMath::RandRange(1, Sounds.Num()) - 1;
		Sound = Sounds[PlayIndex];
	}
	
	Super::Notify(MeshComp, Animation, EventReference);
}

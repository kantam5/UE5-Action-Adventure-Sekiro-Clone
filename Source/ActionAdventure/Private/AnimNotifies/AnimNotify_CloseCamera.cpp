// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AnimNotify_CloseCamera.h"
#include "Characters/PlayerCharacter.h"

void UAnimNotify_CloseCamera::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	APlayerCharacter* Owner = Cast<APlayerCharacter>(MeshComp->GetOwner());
	if (Owner)
	{
		Owner->CloseCameraTimeline.PlayFromStart();
	}
}

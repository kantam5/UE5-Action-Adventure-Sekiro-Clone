// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AnimNotify_RopeVisibility.h"

#include "Characters/PlayerCharacter.h"

void UAnimNotify_RopeVisibility::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (MeshComp->IsValidLowLevel())
	{
		APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner());
		if (OwnerCharacter->IsValidLowLevel())
		{
			OwnerCharacter->ChangeRopeVisibility(bRopeVisiblity);
		}
	}
}

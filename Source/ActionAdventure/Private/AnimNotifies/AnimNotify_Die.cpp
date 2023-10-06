// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AnimNotify_Die.h"

#include "Characters/AA_CharacterBase.h"

void UAnimNotify_Die::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AAA_CharacterBase* Owner = Cast<AAA_CharacterBase>(MeshComp->GetOwner());
	if (Owner)
	{
		Owner->Die();
	}
}

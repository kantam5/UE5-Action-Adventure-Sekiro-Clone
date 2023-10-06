// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AnimNotify_GenericEventByTag.h"

#include "Characters/AA_CharacterBase.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAnimNotify_GenericEventByTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	FGameplayEventData EventPayload;
	EventPayload.Instigator = MeshComp->GetOwner();
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, EventPayload);
}

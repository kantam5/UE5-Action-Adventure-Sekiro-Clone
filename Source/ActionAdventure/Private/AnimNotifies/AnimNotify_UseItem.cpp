// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AnimNotify_UseItem.h"

#include "AbilitySystemBlueprintLibrary.h"

void UAnimNotify_UseItem::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	FGameplayEventData EventPayload;
	EventPayload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(MeshComp->GetOwner());
	EventPayload.Target = MeshComp->GetOwner();
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, EventPayload);

}

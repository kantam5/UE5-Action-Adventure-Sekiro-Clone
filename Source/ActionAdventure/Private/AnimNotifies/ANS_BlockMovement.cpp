// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_BlockMovement.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UANS_BlockMovement::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner()))
	{
		AbilityComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.BlockInput")));
	}
}

void UANS_BlockMovement::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner()))
	{
		AbilityComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.BlockInput")));
	}
}

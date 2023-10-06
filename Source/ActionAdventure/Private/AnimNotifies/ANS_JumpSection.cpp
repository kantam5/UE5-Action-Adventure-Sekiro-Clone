// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_JumpSection.h"

#include "Characters/PlayerCharacter.h"

void UANS_JumpSection::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	APlayerCharacter* Character = Cast<APlayerCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		Character->SetEnableComboPeriod(true);
		Character->SetJumpSectionNotify(this);
	}
}

void UANS_JumpSection::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	APlayerCharacter* Character = Cast<APlayerCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		Character->SetEnableComboPeriod(false);
	}

	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

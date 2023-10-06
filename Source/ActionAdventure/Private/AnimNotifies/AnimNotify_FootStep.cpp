// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AnimNotify_FootStep.h"

#include "Characters/AA_CharacterBase.h"
#include "ActorComponents/FootStepsComponent.h"

void UAnimNotify_FootStep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AAA_CharacterBase* Owner = Cast<AAA_CharacterBase>(MeshComp->GetOwner());
	if (Owner)
	{
		UFootStepsComponent* FootStepsComponent = Owner->GetFootStepsComponent();
		if (FootStepsComponent)
		{
			FootStepsComponent->HandleFootStep(Foot);
		}
	}
}
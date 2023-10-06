// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_CharacterRotation.h"

#include "Characters/PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "ActorComponents/GrapplingHookComponent.h"

UANS_CharacterRotation::UANS_CharacterRotation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UANS_CharacterRotation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (OwnerCharacter->IsValidLowLevel())
	{
		StartRot = OwnerCharacter->GetActorTransform().Rotator();
		EndRot = UKismetMathLibrary::FindLookAtRotation(
			OwnerCharacter->GetActorLocation(),
			OwnerCharacter->GetGrapplingHookComponent()->GetLastFocusedPointLandLoc());

		EndRot.Pitch = 0.f;
		EndRot.Roll = 0.f;
	}
}

void UANS_CharacterRotation::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (OwnerCharacter->IsValidLowLevel())
	{
		Rotation(FrameDeltaTime);
	}
}

void UANS_CharacterRotation::Rotation(float FrameDeltaTime)
{
	float Alpha = CurrentRotationTime / NotifyTotalTime;

	if (Alpha < 1.f)
	{
		FRotator LerpRot = UKismetMathLibrary::RLerp(StartRot, EndRot, Alpha, true);
		
		OwnerCharacter->SetActorRotation(LerpRot);

		CurrentRotationTime += FrameDeltaTime;
	}
}

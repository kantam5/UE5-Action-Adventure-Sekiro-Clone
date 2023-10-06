// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_WideRangeWeaponAttack.h"

#include "Characters/AA_CharacterBase.h"
#include "Actors/WeaponActor.h"

void UANS_WideRangeWeaponAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	AAA_CharacterBase* Character = Cast<AAA_CharacterBase>(MeshComp->GetOwner());
	AWeaponActor* Weapon = Character ? Character->GetWeapon() : nullptr;
	if (Weapon)
	{
		Weapon->BeginWideRangeWeaponAttack(EventTag);
	}
}

void UANS_WideRangeWeaponAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AAA_CharacterBase* Character = Cast<AAA_CharacterBase>(MeshComp->GetOwner());
	AWeaponActor* Weapon = Character ? Character->GetWeapon() : nullptr;
	if (Weapon)
	{
		Weapon->EndWideRangeWeaponAttack();
	}

	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

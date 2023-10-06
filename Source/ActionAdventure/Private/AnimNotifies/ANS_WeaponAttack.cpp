// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_WeaponAttack.h"

#include "Characters/AA_CharacterBase.h"
#include "Actors/WeaponActor.h"

void UANS_WeaponAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	AAA_CharacterBase* Character = Cast<AAA_CharacterBase>(MeshComp->GetOwner());
	AWeaponActor* Weapon = Character ? Character->GetWeapon() : nullptr;
	if (Weapon)
	{
		Weapon->BeginWeaponAttack(EventTag);
	}

}

void UANS_WeaponAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AAA_CharacterBase* Character = Cast<AAA_CharacterBase>(MeshComp->GetOwner());
	AWeaponActor* Weapon = Character ? Character->GetWeapon() : nullptr;
	if (Weapon)
	{
		Weapon->EndWeaponAttack();
	}

	Super::NotifyEnd(MeshComp, Animation, EventReference);

}

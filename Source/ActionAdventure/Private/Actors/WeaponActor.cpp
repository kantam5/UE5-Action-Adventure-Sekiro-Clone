// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/WeaponActor.h"

#include "Components/CapsuleComponent.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/AA_CharacterBase.h"

// Sets default values
AWeaponActor::AWeaponActor()
	: OverlapedActors()
	, AttackEventTag()
	, bIsAttacking(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	RootComponent = CapsuleCollision;
	CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponActor::CapsuleBeginOverlap);

	WideRangeCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("WideRangeCapsuleCollision"));
	WideRangeCollision->SetupAttachment(RootComponent);
	WideRangeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WideRangeCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponActor::CapsuleBeginOverlap);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeaponActor::BeginWeaponAttack(FGameplayTag EventTag)
{
	AttackEventTag = EventTag;
	bIsAttacking = true;

	CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeaponActor::EndWeaponAttack()
{
	bIsAttacking = false;

	CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OverlapedActors.Empty();
}

void AWeaponActor::BeginWideRangeWeaponAttack(FGameplayTag EventTag)
{
	AttackEventTag = EventTag;
	bIsAttacking = true;

	WideRangeCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeaponActor::EndWideRangeWeaponAttack()
{
	bIsAttacking = false;

	WideRangeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OverlapedActors.Empty();
}

void AWeaponActor::CapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((GetOwner() != OtherActor) && bIsAttacking && OverlapedActors.Find(OtherActor))
	{
		AAA_CharacterBase* OwnerCharacter = Cast<AAA_CharacterBase>(GetOwner());
		AAA_CharacterBase* TargetCharacter = Cast<AAA_CharacterBase>(OtherActor);
		if (OwnerCharacter && TargetCharacter)
		{
			if (!OwnerCharacter->IsEnemy(TargetCharacter->GetTeamNumber()))
			{
				return;
			}
		}
		if (TargetCharacter->IsValidLowLevel())
		{
			if (TargetCharacter->IsInvincible())
			{
				return;
			}
		}

		OverlapedActors.Add(OtherActor);

		FGameplayEventData EventPayload;
		EventPayload.Instigator = GetOwner();
		EventPayload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(OtherActor);
		EventPayload.Target = OtherActor;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), AttackEventTag, EventPayload);
		
	}
}

void AWeaponActor::Init()
{
	if (MeshComponent)
	{
		MeshComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/AA_Projectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Characters/AA_CharacterBase.h"
#include "AbilitySystemComponent.h"

AAA_Projectile::AAA_Projectile()
	: Range(0.f)
{
	PrimaryActorTick.bCanEverTick = false;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	RootComponent = SphereCollision;
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
}

void AAA_Projectile::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(Range / ProjectileMovement->InitialSpeed);

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AAA_Projectile::SphereCollisionBeginOverlap);
}

void AAA_Projectile::SphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetInstigator() == OtherActor)
	{
		return;
	}

	AAA_CharacterBase* Target = Cast<AAA_CharacterBase>(OtherActor);
	if (Target)
	{
		if (Target->GetAbilitySystemComponent())
		{
			FGameplayEffectContextHandle EffectContext = Target->GetAbilitySystemComponent()->MakeEffectContext();
			EffectContext.AddSourceObject(GetInstigator());

			Target->ApplyGameplayEffectToSelf(DamageEffect, EffectContext);
		}
	}

	Destroy();
}


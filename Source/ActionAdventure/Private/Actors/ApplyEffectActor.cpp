// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ApplyEffectActor.h"

#include "Components/SphereComponent.h"
#include "Characters/AA_CharacterBase.h"
#include "AbilitySystemComponent.h"

AApplyEffectActor::AApplyEffectActor()
	: LifeTime(0.f)
{
	PrimaryActorTick.bCanEverTick = false;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	RootComponent = SphereCollision;
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AApplyEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(LifeTime);

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AApplyEffectActor::SphereCollisionBeginOverlap);
}

void AApplyEffectActor::SphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
			for (auto Effect : EffectsToApply)
			{
				FGameplayEffectContextHandle EffectContext = Target->GetAbilitySystemComponent()->MakeEffectContext();
				EffectContext.AddSourceObject(GetInstigator());

				Target->ApplyGameplayEffectToSelf(Effect, EffectContext);
			}
			
		}
	}
}


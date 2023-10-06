// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ApplyEffectActor.generated.h"

class UGameplayEffect;
class USphereComponent;

UCLASS()
class ACTIONADVENTURE_API AApplyEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AApplyEffectActor();

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayEffect>> EffectsToApply;

	UPROPERTY(EditAnywhere)
	float LifeTime;

	UFUNCTION()
	void SphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	USphereComponent* SphereCollision;


};

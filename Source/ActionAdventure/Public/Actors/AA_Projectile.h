// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "AA_Projectile.generated.h"

class UProjectileMovementComponent;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class ACTIONADVENTURE_API AAA_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AAA_Projectile();

	UPROPERTY(EditAnywhere)
	float Range;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovement;

	UFUNCTION()
	void SphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	USphereComponent* SphereCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* MeshComponent;
};

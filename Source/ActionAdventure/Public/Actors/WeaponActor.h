// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "WeaponActor.generated.h"

class UCapsuleComponent;

UCLASS()
class ACTIONADVENTURE_API AWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponActor();

	void BeginWeaponAttack(FGameplayTag EventTag);
	void EndWeaponAttack();

	void BeginWideRangeWeaponAttack(FGameplayTag EventTag);
	void EndWideRangeWeaponAttack();

	UFUNCTION()
	void CapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void Init();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCapsuleComponent* CapsuleCollision;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCapsuleComponent* WideRangeCollision;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USkeletalMesh* SkeletalMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* StaticMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* MeshComponent;

	TArray<AActor*> OverlapedActors;

	FGameplayTag AttackEventTag;

	bool bIsAttacking;

};

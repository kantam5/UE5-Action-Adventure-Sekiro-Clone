// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EnemyCharacterBase.h"
#include "MinionCharacter.generated.h"

class UDetectionIcon;
class USplineComponent;
class AItemPickupActor;
class UAA_Item;

UCLASS()
class ACTIONADVENTURE_API AMinionCharacter : public AEnemyCharacterBase
{
	GENERATED_BODY()

public:
	AMinionCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void HandlePoiseDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, AAA_CharacterBase* InstigatorCharacter, AActor* DamageCauser) override;

	FVector GetNextMoveLocation();

protected:
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDetectionIcon> DetectIconClass;
	UPROPERTY()
	UDetectionIcon* DetectIcon;
	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* DetectWidgetComp;

	UPROPERTY(EditAnywhere)
	USplineComponent* SplineComponent;

	TArray<FVector> PatrolPoints;
	int32 CurrentPatrolPoint;

	bool bHealthDepleted;

	UPROPERTY(EditAnywhere, Category = Item)
	TArray<UAA_Item*> DropItems;
	UPROPERTY(EditAnywhere, Category = Item)
	TSubclassOf<AItemPickupActor> ItemPickupActorClass;
	
	virtual void OnHealthAttributeChanged(const FOnAttributeChangeData& Data) override;
	virtual void OnHealthDepleted() override;

	virtual void OnStaminaAttributeChanged(const FOnAttributeChangeData& Data) override;
	virtual void OnStaminaDepleted() override;

	virtual void OnDeathCountAttributeChanged(const FOnAttributeChangeData& Data) override;
	virtual void OnDeathCountDepleted() override;

	virtual void Die() override;

	void InitializeDetectIcon();

	void SpawnItem();

};

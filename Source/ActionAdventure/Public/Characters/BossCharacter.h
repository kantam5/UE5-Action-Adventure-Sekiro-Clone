// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EnemyCharacterBase.h"
#include "BossCharacter.generated.h"

class UBoxComponent;
class AFogWall;

UCLASS()
class ACTIONADVENTURE_API ABossCharacter : public AEnemyCharacterBase
{
	GENERATED_BODY()

public:
	ABossCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnDetectionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void PlayFinishedMontage();

	FORCEINLINE FText GetBossName() const { return BossName; }

protected:
	virtual void OnHealthAttributeChanged(const FOnAttributeChangeData& Data) override;
	virtual void OnHealthDepleted() override;

	virtual void OnStaminaAttributeChanged(const FOnAttributeChangeData& Data) override;
	virtual void OnStaminaDepleted() override;

	virtual void OnDeathCountAttributeChanged(const FOnAttributeChangeData& Data) override;
	virtual void OnDeathCountDepleted() override;

	virtual void OnGroggyMontageComplete() override;

	virtual void Die() override;

	virtual void ResetCharacterToOrigin() override;

	void PlayFinishedGroggyMontage();
	UFUNCTION()
	void OnFinishedGroggyMontageBelndOut(UAnimMontage* AnimMontage, bool trigger);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* DetectionBox;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<AFogWall*> FogWalls;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* FinishedGroggyMontage;
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* FinishedMontage;

	UPROPERTY(EditDefaultsOnly)
	FText BossName;
};

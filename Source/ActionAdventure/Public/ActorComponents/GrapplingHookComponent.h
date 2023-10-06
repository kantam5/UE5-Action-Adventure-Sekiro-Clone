// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "AA_CharacterTypes.h"
#include "GrapplingHookComponent.generated.h"

class APlayerCharacter;
class UAnimMontage;
class AHookablePoint;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONADVENTURE_API UGrapplingHookComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGrapplingHookComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool CanHookThrow() const;

	UAnimMontage* GetHookMontage();

	void PlayInputAnimation();

	UFUNCTION()
	void OnCapsuleOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCapsuleOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FORCEINLINE const FVector& GetLastFocusedPointTrackerLoc() const { return LastFocusedPointTrackerLoc; }
	FORCEINLINE const FVector& GetLastFocusedPointLandLoc() const { return LastFocusedPointLandLoc; }

	FORCEINLINE const FHookMontage* GetCurrentMontage() const { return CurrentMontage; }

	FORCEINLINE const bool IsLastAndCurrentFocusedLocEqual() const { return FocusedPoint == LastFocusedPoint; }

protected:
	virtual void BeginPlay() override;

private:
	TWeakObjectPtr<APlayerCharacter> OwnerCharacter;

	UPROPERTY()
	TArray<AHookablePoint*> HookablePoints;

	UPROPERTY()
	AHookablePoint* FocusedPoint;
	UPROPERTY()
	AHookablePoint* LastFocusedPoint;

	FVector LastFocusedPointTrackerLoc;
	FVector LastFocusedPointLandLoc;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UDataTable* HookJumpMontages;

	FHookMontage* CurrentMontage;

	void UpdateHookeablePoints();
	bool IsCharacterWithinPointBounds(const AHookablePoint* HookablePoint) const;
	bool IsCharacterInFrontOfThePoint(const AHookablePoint* HookablePoint) const;
	bool IsPointVisible(AHookablePoint* HookablePoint) const;

	float CharacterPointDistance(const AHookablePoint* HookablePoint) const;
	void CalcPointFacingRate(AHookablePoint* HookablePoint, TMap<int, float>& PointsFacingRate) const;
	void CalcBestFacingPointRate(TMap<int, float>& PointsFacingRate);

};

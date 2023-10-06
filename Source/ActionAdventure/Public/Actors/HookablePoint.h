// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UI/Hookable/HookablePointWidget.h"
#include "HookablePoint.generated.h"

class UBillboardComponent;
class UBoxComponent;
class UArrowComponent;
class UHookablePointWidget;
class USphereComponent;

UCLASS()
class ACTIONADVENTURE_API AHookablePoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AHookablePoint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor_Var")
	float LineTraceLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor_Var")
	float DepthLocOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor_Var")
	float MaxUpdateDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor_Var")
	float MinUpdateDistance;

	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override;

	void CustomUpdate(float CharacterPointDistance);
	void SetCharacterFocused(bool InState);

	void HidePoint();
	void PlayInputAnimation();

	FORCEINLINE bool GetIsValid() const { return bIsValid; }
	FORCEINLINE bool GetIsHookable() const { return bIsHookable; }
	
	FORCEINLINE const FVector& GetLandLoc() const { return LandPointLoc; }

	FORCEINLINE FVector GetTrackerLoc() const { return UITracker->GetComponentLocation(); }

	FORCEINLINE const FVector2D& GetScreenPos() const { return UITracker->GetScreenPos(); }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBillboardComponent* RootPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBillboardComponent* HookPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBillboardComponent* LandPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UHookablePointWidget* UITracker;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* ActivableArea;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* VisibilityCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* DirectionalArrow;

	FVector LandPointLoc;

	bool bIsValid;
	bool bIsHookable;

	void CalculateHookAndLandLoc();

};

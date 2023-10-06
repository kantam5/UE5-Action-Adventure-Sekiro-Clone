// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "UI/Hookable/HookablePointTracker.h"
#include "UI/Hookable/HookablePointTrackerMovable.h"
#include "HookablePointWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UHookablePointWidget : public UWidgetComponent
{
	GENERATED_BODY()

/*--------------------Variables---------------------*/
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWidgetComp_Var | Widget")
	TSubclassOf<UHookablePointTrackerMovable> MovableClass;

	UPROPERTY()
	UHookablePointTracker* Tracker;
	UPROPERTY()
	UHookablePointTrackerMovable* MovableTracker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWidgetComp_Var | Material")
	float StartUpdateDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWidgetComp_Var | Material")
	float ScaleRateMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWidgetComp_Var | Material")
	float ScreenMarginOffset;

	FVector2D ScreenPos;
	FVector2D FixedScreenPos;

	bool OutScreenBounds;

/*--------------------Methods---------------------*/
public:
	UHookablePointWidget(const FObjectInitializer& ObjectInitializer);

	void CustomInit();
	void CustomUpdate(const FVector& PointWorldPos, float CharacterPointDistance);

	void SetCharacterFocused(bool InState);
	void HideTrackers();
	bool CheckIfIsHookable() const;
	void PlayInputAnimation();

	UFUNCTION()
	void OnTrackerUnhide(UHookablePointTracker* InTracker);

	void ConstrainScreenBounds(const FVector& PointWorldPos);
	bool TrackerActive() const;
	void VisibilityBehavior();

/*--------------------Getters Setters---------------------*/
	FORCEINLINE const FVector2D& GetScreenPos() const { return ScreenPos; }

};

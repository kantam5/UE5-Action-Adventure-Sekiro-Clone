// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Hookable/HookablePointTracker.h"
#include "HookablePointTrackerMovable.generated.h"

class UImage;

UCLASS()
class ACTIONADVENTURE_API UHookablePointTrackerMovable : public UHookablePointTracker
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* TrackerArrow;

public:
	UHookablePointTrackerMovable(const FObjectInitializer& ObjectInitializer);

	virtual void InitWidget(float InStartUpdateDistane, float InScaleRateMultiplier) override;

	void CustomUpdate(const FVector2D& ScreenPos, const FVector2D& FixedScreenPos, float CharacterPointDistance);

private:
	void UpdateTrackerArrow(const FVector2D& ScreenPos, const FVector2D& FixedScreenPos);
};

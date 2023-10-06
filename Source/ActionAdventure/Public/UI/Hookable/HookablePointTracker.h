// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HookablePointTracker.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTrackerUnhide, UHookablePointTracker*, tracker);

class UVerticalBox;
class UImage;
class UWidgetAnimation;

UCLASS()
class ACTIONADVENTURE_API UHookablePointTracker : public UUserWidget
{
	GENERATED_BODY()

/*--------------------Variables---------------------*/
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables | Material")
	FLinearColor MaxScaleColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables | Material")
	FLinearColor NormalScaleColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables | Material")
	float MaxScaleValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables | Material")
	float MinScaleValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables | Material")
	float RingRadiusOffset;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* Root;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* TrackerPoint;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* InputAnim;

	UPROPERTY()
	UMaterialInstanceDynamic* TrackerPointMat;

	FTrackerUnhide UnhideDelegate;

	float StartUpdateDistance;
	float ScaleRateMultiplier;
	bool Focused;
	bool IsTrackerFull;

/*--------------------Methods---------------------*/
public:
	UHookablePointTracker(const FObjectInitializer& ObjectInitializer);

	virtual void InitWidget(float InStartUpdateDistane, float InScaleRateMultiplier);

	void InitMaterial();

	void CustomUpdate(float CharacterPointDistance);

	void UnhideTracker();

	void ResetWidget();

	void PlayInputAnimation();

protected:
	void UpdatetTrackerPointMaterial(float CharacterPointDistance);

/*--------------------Getters Setters---------------------*/
public:
	FORCEINLINE void SetFocused(bool InState) { Focused = InState; }
	FORCEINLINE bool GetFocused() const { return Focused; }

	FORCEINLINE bool GetIsTrackerFull() const { return IsTrackerFull; }

	FORCEINLINE FTrackerUnhide& GetUnhideDelegate() { return UnhideDelegate; }
};

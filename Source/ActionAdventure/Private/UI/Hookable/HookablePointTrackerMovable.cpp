// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Hookable/HookablePointTrackerMovable.h"

#include "Components/VerticalBox.h"
#include "Components/Image.h"

UHookablePointTrackerMovable::UHookablePointTrackerMovable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UHookablePointTrackerMovable::InitWidget(float InStartUpdateDistane, float InScaleRateMultiplier)
{
	StartUpdateDistance = InStartUpdateDistane;
	ScaleRateMultiplier = InScaleRateMultiplier;

	InitMaterial();

	SetRenderOpacity(0.f);
	Root->SetRenderOpacity(1.f);
	TrackerArrow->SetRenderOpacity(0.0f);

	AddToViewport();
}

void UHookablePointTrackerMovable::CustomUpdate(const FVector2D& ScreenPos, const FVector2D& FixedScreenPos, float CharacterPointDistance)
{
	if (CharacterPointDistance <= StartUpdateDistance)
	{
		UpdatetTrackerPointMaterial(CharacterPointDistance);
	}

	SetPositionInViewport(ScreenPos);

	UpdateTrackerArrow(ScreenPos, FixedScreenPos);
}

void UHookablePointTrackerMovable::UpdateTrackerArrow(const FVector2D& ScreenPos, const FVector2D& FixedScreenPos)
{
	FVector2D ArrowPos = ScreenPos - FVector2D(0.0f, TrackerArrow->GetDesiredSize().Y);

	float ArrowFacingAngle = FMath::RadiansToDegrees(FMath::Atan2(
		FixedScreenPos.Y - ArrowPos.Y, FixedScreenPos.X - ArrowPos.X));

	Root->SetRenderTransformAngle(ArrowFacingAngle + 90.f);

	FVector2D TrackerToPoint = FixedScreenPos - ScreenPos;
	float DistanceToPoint = TrackerToPoint.SizeSquared();

	if (DistanceToPoint > 10000.f)
	{
		TrackerArrow->SetRenderOpacity(1.f);
	}
	else
	{
		TrackerArrow->SetRenderOpacity(0.f);
	}
}

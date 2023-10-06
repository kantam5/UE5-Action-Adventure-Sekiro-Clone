// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Hookable/HookablePointTracker.h"

#include "Components/VerticalBox.h"
#include "Components/Image.h"

UHookablePointTracker::UHookablePointTracker(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	TrackerPointMat(nullptr),
	StartUpdateDistance(0.f),
	ScaleRateMultiplier(0.f),
	Focused(false),
	IsTrackerFull(false)
{
}

void UHookablePointTracker::InitWidget(float InStartUpdateDistane, float InScaleRateMultiplier)
{
	StartUpdateDistance = InStartUpdateDistane;
	ScaleRateMultiplier = InScaleRateMultiplier;

	InitMaterial();

	SetRenderOpacity(0.f);
	Root->SetRenderOpacity(1.f);
}

void UHookablePointTracker::InitMaterial()
{
	TrackerPointMat = TrackerPoint->GetDynamicMaterial();

	if (TrackerPointMat)
	{
		TrackerPointMat->SetScalarParameterValue("Radius", MinScaleValue);
		TrackerPointMat->SetScalarParameterValue("InnerRadius", MaxScaleValue + RingRadiusOffset);
		TrackerPointMat->SetScalarParameterValue("OuterRadius", MaxScaleValue + RingRadiusOffset * 2.f);
		TrackerPointMat->SetVectorParameterValue("Color", NormalScaleColor);
	}
}

void UHookablePointTracker::CustomUpdate(float CharacterPointDistance)
{
	if (CharacterPointDistance <= StartUpdateDistance)
	{
		UpdatetTrackerPointMaterial(CharacterPointDistance);
	}
}

void UHookablePointTracker::UnhideTracker()
{
	if (GetRenderOpacity() == 0.f)
	{
		SetRenderOpacity(1.f);
		
		UnhideDelegate.Broadcast(this);
	}
}

void UHookablePointTracker::ResetWidget()
{
	Focused = false;

	if (TrackerPointMat)
	{
		TrackerPointMat->SetScalarParameterValue("Radius", MinScaleValue);
		TrackerPointMat->SetVectorParameterValue("Color", NormalScaleColor);
	}

	TrackerPoint->SetRenderScale(FVector2D(1.f, 1.f));
	TrackerPoint->SetRenderOpacity(1.f);
	Root->SetRenderOpacity(1.f);
}

void UHookablePointTracker::PlayInputAnimation()
{
	PlayAnimation(InputAnim);
}

void UHookablePointTracker::UpdatetTrackerPointMaterial(float CharacterPointDistance)
{
	float MaterialScaleRate = 0.f;

	MaterialScaleRate = -1.f * (CharacterPointDistance / StartUpdateDistance) + 1.f;
	MaterialScaleRate *= ScaleRateMultiplier;
	
	TrackerPointMat->SetScalarParameterValue("Radius", FMath::Clamp(MaterialScaleRate, MinScaleValue, MaxScaleValue));

	// 캐릭터가 갈고리를 걸 수 있는 거리라면 색을 변환
	if (MaterialScaleRate >= MaxScaleValue)
	{
		TrackerPointMat->SetVectorParameterValue("Color", MaxScaleColor);
		IsTrackerFull = true;
	}
	else
	{
		TrackerPointMat->SetVectorParameterValue("Color", NormalScaleColor);
		IsTrackerFull = false;
	}
}

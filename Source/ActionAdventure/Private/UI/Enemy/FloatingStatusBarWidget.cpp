// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Enemy/FloatingStatusBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"

void UFloatingStatusBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UFloatingStatusBarWidget::DeductDeathCount()
{
	if (DeathCountNum == 2)
	{
		DeathCountSecond->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f));
		DeathCountNum--;
	}
	else
	{
		DeathCountFirst->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f));
		DeathCountNum--;
	}
}

void UFloatingStatusBarWidget::AddDeathCount()
{
	DeathCountNum++;
	if (DeathCountNum == 1)
	{
		DeathCountFirst->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f));
	}
	else if (DeathCountNum == 2)
	{
		DeathCountSecond->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f));
	}
}

void UFloatingStatusBarWidget::SetDeathCountNumber(float InDeathCount)
{
	if (FMath::IsNearlyEqual(InDeathCount, 2.f))
	{
		MaxDeathCountNum = 2;
		DeathCountNum = 2;
		DeathCountFirst->SetVisibility(ESlateVisibility::Visible);
		DeathCountSecond->SetVisibility(ESlateVisibility::Visible);
	}
	else if (FMath::IsNearlyEqual(InDeathCount, 1.f))
	{
		MaxDeathCountNum = 1;
		DeathCountNum = 1;
		DeathCountFirst->SetVisibility(ESlateVisibility::Visible);
		DeathCountSecond->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UFloatingStatusBarWidget::SetHealthPercentage(float HealthPercentage)
{
	ProgressBar_Health->SetPercent(HealthPercentage);
}

void UFloatingStatusBarWidget::SetStaminaPercentage(float StaminaPercentage)
{
	ProgressBar_Stamina->SetPercent(1.0 - StaminaPercentage);
}

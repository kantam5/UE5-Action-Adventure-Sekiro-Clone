// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/IdolWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Player/AA_PlayerController.h"
#include "Actors/CheckPoint.h"
#include "UI/Player/IdolTravelList.h"

void UIdolWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Rest->OnClicked.AddUniqueDynamic(this, &UIdolWidget::OnRestButtonClicked);
	Button_Travel->OnClicked.AddUniqueDynamic(this, &UIdolWidget::OnTravelButtonClicked);
	Button_Back->OnClicked.AddUniqueDynamic(this, &UIdolWidget::OnBackButtonClicked);

	Button_Rest->OnHovered.AddUniqueDynamic(this, &UIdolWidget::OnRestButtonHovered);
	Button_Travel->OnHovered.AddUniqueDynamic(this, &UIdolWidget::OnTravelButtonHovered);
	Button_Back->OnHovered.AddUniqueDynamic(this, &UIdolWidget::OnBackButtonHovered);

	Button_Rest->OnUnhovered.AddUniqueDynamic(this, &UIdolWidget::OnRestButtonUnhovered);
	Button_Travel->OnUnhovered.AddUniqueDynamic(this, &UIdolWidget::OnTravelButtonUnhovered);
	Button_Back->OnUnhovered.AddUniqueDynamic(this, &UIdolWidget::OnBackButtonUnhovered);

	IdolTravelList->SetVisibility(ESlateVisibility::Hidden);
}

void UIdolWidget::SetIdolData(ACheckPoint* InIdol)
{
	Idol = InIdol;
	if (Idol)
	{
		TextBlock_IdolName->SetText(Idol->IdolName);
	}
	
}

void UIdolWidget::OnRestButtonClicked()
{
	if (Idol)
	{
		Idol->Rest();
		IdolTravelList->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UIdolWidget::OnTravelButtonClicked()
{
	IdolTravelList->SetVisibility(ESlateVisibility::Visible);
}

void UIdolWidget::OnBackButtonClicked()
{
	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		PlayerController->EndIdolUI();
	}
}

void UIdolWidget::OnRestButtonHovered()
{
	Button_Rest->SetBackgroundColor(ButtonHoveredColor);
}

void UIdolWidget::OnTravelButtonHovered()
{
	Button_Travel->SetBackgroundColor(ButtonHoveredColor);
}

void UIdolWidget::OnBackButtonHovered()
{
	Button_Back->SetBackgroundColor(ButtonHoveredColor);
}

void UIdolWidget::OnRestButtonUnhovered()
{
	Button_Rest->SetBackgroundColor(FLinearColor(0.0f, 0.f, 0.f, 0.f));
}

void UIdolWidget::OnTravelButtonUnhovered()
{
	Button_Travel->SetBackgroundColor(FLinearColor(0.0f, 0.f, 0.f, 0.f));
}

void UIdolWidget::OnBackButtonUnhovered()
{
	Button_Back->SetBackgroundColor(FLinearColor(0.0f, 0.f, 0.f, 0.f));
}

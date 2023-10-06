// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IdolWidget.generated.h"

class UTextBlock;
class UButton;
class ACheckPoint;
class UIdolTravelList;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UIdolWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_IdolName;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Rest;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Travel;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Back;

	UPROPERTY(meta = (BindWidget))
	UIdolTravelList* IdolTravelList;

	UPROPERTY()
	ACheckPoint* Idol;

	const FLinearColor ButtonHoveredColor = FLinearColor(1.0f, 0.340347f, 0.059842f, 1.f);

	virtual void NativeConstruct() override;

	void SetIdolData(ACheckPoint* InIdol);

	UFUNCTION()
	void OnRestButtonClicked();
	UFUNCTION()
	void OnTravelButtonClicked();
	UFUNCTION()
	void OnBackButtonClicked();

	UFUNCTION()
	void OnRestButtonHovered();
	UFUNCTION()
	void OnTravelButtonHovered();
	UFUNCTION()
	void OnBackButtonHovered();
	UFUNCTION()
	void OnRestButtonUnhovered();
	UFUNCTION()
	void OnTravelButtonUnhovered();
	UFUNCTION()
	void OnBackButtonUnhovered();
};

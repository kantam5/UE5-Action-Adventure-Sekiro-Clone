// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FloatingStatusBarWidget.generated.h"

class UImage;
class UCanvasPanel;
class UProgressBar;

UCLASS()
class ACTIONADVENTURE_API UFloatingStatusBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void DeductDeathCount();
	void AddDeathCount();

	void SetDeathCountNumber(float InDeathCount);

	void SetHealthPercentage(float HealthPercentage);

	void SetStaminaPercentage(float StaminaPercentage);

protected:
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* CanvasPanel;

	UPROPERTY(meta = (BindWidget))
	UImage* DeathCountFirst;

	UPROPERTY(meta = (BindWidget))
	UImage* DeathCountSecond;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar_Health;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar_Stamina;

	int32 MaxDeathCountNum;
	int32 DeathCountNum;


};

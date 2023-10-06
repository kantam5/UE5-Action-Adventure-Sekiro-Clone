// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConfirmWidget.generated.h"

class UTextBlock;
class UButton;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UConfirmWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_Confirm;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_OK;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Cancel;

};

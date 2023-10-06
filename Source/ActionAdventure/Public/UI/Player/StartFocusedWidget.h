// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartFocusedWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UStartFocusedWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_Focused;

	void SetFocusedText(FText InText);
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScreenOverlayWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UScreenOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* ExecuteAnim;

	void PlayExecuteAnimation();
	
};

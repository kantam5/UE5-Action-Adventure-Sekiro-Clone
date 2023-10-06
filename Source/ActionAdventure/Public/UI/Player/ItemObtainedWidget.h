// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemObtainedWidget.generated.h"

class UTextBlock;
class UImage;
class UButton;
class UAA_Item;

UCLASS()
class ACTIONADVENTURE_API UItemObtainedWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowObtainedItemUI(UAA_Item* InItemType);

	UFUNCTION()
	void OnButtonClicked();
	
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_ItemName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_ItemDescription;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_ItemIcon;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_OK;

	UPROPERTY()
	UAA_Item* ItemType;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeInAnim;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOutAnim;

	FTimerHandle FadeOutHandle;
};

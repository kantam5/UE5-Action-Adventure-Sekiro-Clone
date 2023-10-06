// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "ItemDurationBox.generated.h"

class UHorizontalBox;
class UImage;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UItemDurationBox : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* HorizontalBox_ItemDuration;

	UPROPERTY()
	TMap<FGameplayTag, UImage*> ItemImageMap;

	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, FSlateBrush> ItemIconMap;
	
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnPotionTagChanged(FGameplayTag CallbackTag, int32 NewCount);
};

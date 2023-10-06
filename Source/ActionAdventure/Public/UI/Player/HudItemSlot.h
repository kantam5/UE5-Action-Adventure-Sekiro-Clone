// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HudItemSlot.generated.h"

class UImage;
class UTextBlock;
class UAA_Item;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UHudItemSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UImage* Image_ItemIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_ItemCount;

	UPROPERTY()
	UAA_Item* Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FPrimaryAssetType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	bool bIsConsumable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	bool bIsSkill;

public:
	virtual void NativeConstruct() override;

	void UpdateSlot(int32 SlotNumber);

};

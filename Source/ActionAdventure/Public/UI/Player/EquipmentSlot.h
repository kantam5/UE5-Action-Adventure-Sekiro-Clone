// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AA_GameTypes.h"
#include "EquipmentSlot.generated.h"

class UButton;
class UImage;
class UAA_Item;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UEquipmentSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Equip;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_ItemIcon;

	UPROPERTY(EditAnywhere)
	FAA_ItemSlot EquipSlot;
	UPROPERTY(EditAnywhere)
	int32 MinEquippedItems;

	UPROPERTY()
	UAA_Item* Item;

public:
	virtual void NativeConstruct() override;

	void RefreshItem();

	void UpdateEquipmentslot(UAA_Item* InItem);

	UFUNCTION()
	void OnButtonClicked();
	UFUNCTION()
	void OnButtonHovered();

	UFUNCTION()
	void SlottedItemChanged(FAA_ItemSlot ItemSlot, UAA_Item* InItem);
};

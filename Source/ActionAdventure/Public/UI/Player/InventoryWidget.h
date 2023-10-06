// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AA_GameTypes.h"
#include "InventoryWidget.generated.h"

class UCanvasPanel;
class UImage;
class UTextBlock;
class UButton;
class UWidgetSwitcher;
class UAA_Item;
class USelectEquipmentList;
class UEquipmentSlot;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SetItemDescript(UAA_Item* InItem);

	void ShowSelectEquipmentList(FAA_ItemSlot EquipSlot);
	void EndSelectEquipmentList();

	UFUNCTION()
	void OnEquipmentButtonClicked();
	UFUNCTION()
	void OnInventoryButtonClicked();
	UFUNCTION()
	void OnSkillButtonClicked();
	UFUNCTION()
	void OnKeyButtonClicked();

	FORCEINLINE UEquipmentSlot* GetCurrentEquipmentSlot() { return CurrentEquipmentSlot; }
	FORCEINLINE void SetCurrentEquipmentSlot(UEquipmentSlot* InEquipmentSlot) { CurrentEquipmentSlot = InEquipmentSlot; }

protected:
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* CanvasPanel_TopMenu;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Equipment;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Inventory;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Skills;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Keys;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_FocusedItemName;
	UPROPERTY(meta = (BindWidget))
	UImage* Image_FocusedItemIcon;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_ItemCount;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_ItemDescription;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WidgetSwitcher_InventoryTab;

	UPROPERTY(meta = (BindWidget))
	USelectEquipmentList* SelectEquipmentList;

	UPROPERTY()
	UEquipmentSlot* CurrentEquipmentSlot;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SelectEquipmentList.generated.h"

class UListView;
class UAA_Item;
class UButton;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API USelectEquipmentList : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void AddItems(FPrimaryAssetType InItemType);

	UFUNCTION()
	void OnBackButtonClicked();

	UFUNCTION()
	void OnInventoryItemChanged(bool bAdded, UAA_Item* Item);

	UFUNCTION(BlueprintCallable)
	void OnItemHovered(UObject* InItem, bool bIsHovered);

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Back;

	UPROPERTY(meta = (BindWidget))
	UListView* ListView_ItemList;

	FPrimaryAssetType ItemType;


};

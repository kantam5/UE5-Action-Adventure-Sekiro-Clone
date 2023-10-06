// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "InventorySlot.generated.h"

class UImage;
class UTextBlock;
class UAA_Item;
class UConfirmWidget;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UInventorySlot : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void SetSlotInfo(UAA_Item* InItem);

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* Image_ItemIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_ItemName;

	UPROPERTY()
	UAA_Item* Item;

};

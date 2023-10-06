// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/InventorySlot.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "DataAssets/AA_Item.h"
#include "UI/Player/ConfirmWidget.h"
#include "Components/Button.h"
#include "Player/AA_PlayerController.h"

void UInventorySlot::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	UAA_Item* InItem = Cast<UAA_Item>(ListItemObject);
	if (InItem)
	{
		SetSlotInfo(InItem);
	}
}

FReply UInventorySlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (Item->IsConsumable())
	{
		AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetOwningPlayer());
		if (PlayerController)
		{
			PlayerController->ShowConfirmUI(Item);
		}
	}

	return Reply;
}

void UInventorySlot::SetSlotInfo(UAA_Item* InItem)
{
	Item = InItem;
	Image_ItemIcon->SetBrush(InItem->ItemIcon);
	Image_ItemIcon->SetDesiredSizeOverride(FVector2D(100.f, 100.f));
	TextBlock_ItemName->SetText(InItem->ItemName);
}

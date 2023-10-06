// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/SelectEquipmentSlot.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "DataAssets/AA_Item.h"
#include "Components/Button.h"
#include "Player/AA_PlayerController.h"
#include "UI/Player/InventoryWidget.h"
#include "UI/Player/EquipmentSlot.h"

void USelectEquipmentSlot::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	UAA_Item* InItem = Cast<UAA_Item>(ListItemObject);
	if (InItem)
	{
		SetSlotInfo(InItem);
	}

}

FReply USelectEquipmentSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		if (PlayerController->GetInventoryWidget()->GetCurrentEquipmentSlot()->Item == Item)
		{
			PlayerController->SetSlottedItem(PlayerController->GetInventoryWidget()->GetCurrentEquipmentSlot()->EquipSlot, nullptr);
		}
		else
		{
			PlayerController->GetInventoryWidget()->GetCurrentEquipmentSlot()->UpdateEquipmentslot(Item);
		}
		
		PlayerController->GetInventoryWidget()->EndSelectEquipmentList();
	}

	return Reply;
}

void USelectEquipmentSlot::SetSlotInfo(UAA_Item* InItem)
{
	Item = InItem;
	Image_ItemIcon->SetBrush(InItem->ItemIcon);
	Image_ItemIcon->SetDesiredSizeOverride(FVector2D(100.f, 100.f));
	TextBlock_ItemName->SetText(InItem->ItemName);
}


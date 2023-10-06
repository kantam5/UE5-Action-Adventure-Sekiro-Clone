// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/SelectEquipmentList.h"

#include "Components/ListView.h"
#include "Components/Button.h"
#include "Player/AA_PlayerController.h"
#include "DataAssets/AA_Item.h"
#include "UI/Player/InventoryWidget.h"

void USelectEquipmentList::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Back->OnClicked.AddUniqueDynamic(this, &USelectEquipmentList::OnBackButtonClicked);

	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		PlayerController->OnInventoryItemChanged.AddUniqueDynamic(this, &USelectEquipmentList::OnInventoryItemChanged);
	}

}

void USelectEquipmentList::AddItems(FPrimaryAssetType InItemType)
{
	ItemType = InItemType;

	ListView_ItemList->ClearListItems();

	TArray<UAA_Item*> Items;

	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		PlayerController->GetInventoryItems(Items, InItemType);
	}

	for (UAA_Item* Item : Items)
	{
		if (Item->ItemType == InItemType)
		{
			ListView_ItemList->AddItem(Item);
		}
	}
}

void USelectEquipmentList::OnBackButtonClicked()
{
	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		PlayerController->GetInventoryWidget()->EndSelectEquipmentList();
	}
}

void USelectEquipmentList::OnInventoryItemChanged(bool bAdded, UAA_Item* Item)
{
	if (Item->ItemType == ItemType)
	{
		if (bAdded)
		{
			ListView_ItemList->AddItem(Item);
		}
		else
		{
			ListView_ItemList->RemoveItem(Item);
		}
	}
}

void USelectEquipmentList::OnItemHovered(UObject* InItem, bool bIsHovered)
{
	if (bIsHovered)
	{
		UAA_Item* Item = Cast<UAA_Item>(InItem);
		if (Item)
		{
			AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetOwningPlayer());
			if (PlayerController)
			{
				PlayerController->GetInventoryWidget()->SetItemDescript(Item);
			}
		}
	}
}

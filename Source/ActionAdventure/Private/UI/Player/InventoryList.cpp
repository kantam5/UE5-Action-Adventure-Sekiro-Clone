// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/InventoryList.h"

#include "Components/ListView.h"
#include "Player/AA_PlayerController.h"
#include "DataAssets/AA_Item.h"
#include "UI/Player/InventoryWidget.h"

void UInventoryList::NativeConstruct()
{
	Super::NativeConstruct();

	AddItems();
	
	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		PlayerController->OnInventoryItemChanged.AddUniqueDynamic(this, &UInventoryList::OnInventoryItemChanged);
	}

}

void UInventoryList::AddItems()
{
	ListView_ItemList->ClearListItems();

	for (FPrimaryAssetType Type : ItemTypes)
	{
		TArray<UAA_Item*> Items;
		AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetOwningPlayer());
		if (PlayerController)
		{
			PlayerController->GetInventoryItems(Items, Type);
		}

		for (UAA_Item* Item : Items)
		{
			if (Item->ItemType == Type)
			{
				ListView_ItemList->AddItem(Item);
			}
		}
	}
}

void UInventoryList::OnInventoryItemChanged(bool bAdded, UAA_Item* Item)
{
	if(ItemTypes.Find(Item->ItemType))
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

void UInventoryList::OnItemHovered(UObject* InItem, bool bIsHovered)
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

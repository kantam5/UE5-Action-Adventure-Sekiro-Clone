// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/InventoryGrid.h"

#include "UI/Player/InventorySlot.h"
#include "Player/AA_PlayerController.h"
#include "DataAssets/AA_Item.h"
#include "Components/UniformGridPanel.h"

void UInventoryGrid::NativeConstruct()
{
	Super::NativeConstruct();

	AddSlots();

	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		PlayerController->OnInventoryItemChanged.AddDynamic(this, &UInventoryGrid::OnInventoryItemChanged);
	}

	ItemRow = 0;
}

void UInventoryGrid::AddSlots()
{
	UniformGridPanel_ItemGrid->ClearChildren();

	TArray<UAA_Item*> Items;

	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		PlayerController->GetInventoryItems(Items, ItemType);
	}

	for (UAA_Item* Item : Items)
	{
		if (Item->ItemType == ItemType)
		{
			UInventorySlot* InventorySlot = CreateWidget<UInventorySlot>(GetOwningPlayer(), SlotClass);
			if (InventorySlot)
			{
				UniformGridPanel_ItemGrid->AddChildToUniformGrid(InventorySlot, ItemRow, 0);
				ItemRow++;
			}
		}
	}
}

void UInventoryGrid::OnInventoryItemChanged(bool bAdded, UAA_Item* Item)
{
	AddSlots();
}

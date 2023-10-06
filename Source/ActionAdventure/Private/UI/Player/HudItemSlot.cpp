// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/HudItemSlot.h"

#include "DataAssets/AA_AssetManager.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Player/AA_PlayerController.h"
#include "AA_GameTypes.h"
#include "DataAssets/AA_Item.h"

void UHudItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (ItemType == UAA_AssetManager::SkillItemType)
	{
		TextBlock_ItemCount->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UHudItemSlot::UpdateSlot(int32 SlotNumber)
{
	FAA_ItemSlot ItemSlot = FAA_ItemSlot(ItemType, SlotNumber);

	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		Item = PlayerController->GetSlottedItem(ItemSlot);
		if (Item)
		{
			Image_ItemIcon->SetVisibility(ESlateVisibility::Visible);
			Image_ItemIcon->SetBrush(Item->ItemIcon);
			if (Item->ItemType == UAA_AssetManager::PotionItemType)
			{
				TextBlock_ItemCount->SetVisibility(ESlateVisibility::Visible);
				TextBlock_ItemCount->SetText(FText::FromString(FString::Printf(TEXT("%d"), PlayerController->GetInventoryItemCount(Item))));
			}
		}
		else
		{
			Image_ItemIcon->SetVisibility(ESlateVisibility::Hidden);
			TextBlock_ItemCount->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	
}

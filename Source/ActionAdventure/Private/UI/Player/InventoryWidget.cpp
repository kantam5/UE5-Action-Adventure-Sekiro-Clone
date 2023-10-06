// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/InventoryWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "DataAssets/AA_Item.h"
#include "Player/AA_PlayerController.h"
#include "AA_GameTypes.h"
#include "Components/CanvasPanel.h"
#include "UI/Player/SelectEquipmentList.h"
#include "DataAssets/AA_SkillItem.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Equipment->OnHovered.AddUniqueDynamic(this, &UInventoryWidget::OnEquipmentButtonClicked);
	Button_Inventory->OnHovered.AddUniqueDynamic(this, &UInventoryWidget::OnInventoryButtonClicked);
	Button_Skills->OnHovered.AddUniqueDynamic(this, &UInventoryWidget::OnSkillButtonClicked);
	Button_Keys->OnHovered.AddUniqueDynamic(this, &UInventoryWidget::OnKeyButtonClicked);
}

void UInventoryWidget::SetItemDescript(UAA_Item* InItem)
{
	if (InItem)
	{
		Text_FocusedItemName->SetText(InItem->ItemName);
		Image_FocusedItemIcon->SetBrush(InItem->ItemIcon);

		AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetOwningPlayer());
		if (InItem->ItemType == UAA_AssetManager::PotionItemType && PlayerController)
		{
			FAA_ItemData ItemData;
			PlayerController->GetInventoryItemData(InItem, ItemData);

			FString ItemCountString = FString::Printf(TEXT("Uses %d"), ItemData.ItemCount);
			Text_ItemCount->SetText(FText::FromString(ItemCountString));
		}
		else if (InItem->ItemType == UAA_AssetManager::KeyItemType)
		{
			FString ItemString = FString::Printf(TEXT(""));
			Text_ItemCount->SetText(FText::FromString(ItemString));
		}
		else
		{
			FString ManaCostString = FString::Printf(TEXT("Mana Cost %d"), InItem->ManaCost);
			Text_ItemCount->SetText(FText::FromString(ManaCostString));
		}

		Text_ItemDescription->SetText(InItem->ItemDescription);
	}
}

void UInventoryWidget::ShowSelectEquipmentList(FAA_ItemSlot EquipSlot)
{
	CanvasPanel_TopMenu->SetVisibility(ESlateVisibility::Hidden);
	WidgetSwitcher_InventoryTab->SetVisibility(ESlateVisibility::Hidden);

	SelectEquipmentList->SetVisibility(ESlateVisibility::Visible);
	SelectEquipmentList->AddItems(EquipSlot.ItemType);
}

void UInventoryWidget::EndSelectEquipmentList()
{
	CanvasPanel_TopMenu->SetVisibility(ESlateVisibility::Visible);
	WidgetSwitcher_InventoryTab->SetVisibility(ESlateVisibility::Visible);

	SelectEquipmentList->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryWidget::OnEquipmentButtonClicked()
{
	WidgetSwitcher_InventoryTab->SetActiveWidgetIndex(0);
}

void UInventoryWidget::OnInventoryButtonClicked()
{
	WidgetSwitcher_InventoryTab->SetActiveWidgetIndex(1);
}

void UInventoryWidget::OnSkillButtonClicked()
{
	WidgetSwitcher_InventoryTab->SetActiveWidgetIndex(2);
}

void UInventoryWidget::OnKeyButtonClicked()
{
	WidgetSwitcher_InventoryTab->SetActiveWidgetIndex(3);
}

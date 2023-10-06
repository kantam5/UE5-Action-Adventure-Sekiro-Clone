// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/EquipmentSlot.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Player/AA_PlayerController.h"
#include "UI/Player/InventoryWidget.h"
#include "DataAssets/AA_Item.h"

void UEquipmentSlot::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Equip->OnClicked.AddUniqueDynamic(this, &UEquipmentSlot::OnButtonClicked);
	Button_Equip->OnHovered.AddUniqueDynamic(this, &UEquipmentSlot::OnButtonHovered);

	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		PlayerController->OnSlottedItemChanged.AddUniqueDynamic(this, &UEquipmentSlot::SlottedItemChanged);
	}

	RefreshItem();
}

void UEquipmentSlot::RefreshItem()
{
	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		Item = PlayerController->GetSlottedItem(EquipSlot);
	}

	if (Item)
	{
		Image_ItemIcon->SetVisibility(ESlateVisibility::Visible);
		Image_ItemIcon->SetBrush(Item->ItemIcon);
		Image_ItemIcon->SetDesiredSizeOverride(FVector2D(180.f, 180.f));
	}
	else
	{
		Image_ItemIcon->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UEquipmentSlot::UpdateEquipmentslot(UAA_Item* InItem)
{
	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		PlayerController->SetSlottedItem(EquipSlot, InItem);
		RefreshItem();
	}
}

void UEquipmentSlot::OnButtonClicked()
{
	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		PlayerController->GetInventoryWidget()->ShowSelectEquipmentList(EquipSlot);
		PlayerController->GetInventoryWidget()->SetCurrentEquipmentSlot(this);
	}
}

void UEquipmentSlot::OnButtonHovered()
{
	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		PlayerController->GetInventoryWidget()->SetItemDescript(Item);
	}
}

void UEquipmentSlot::SlottedItemChanged(FAA_ItemSlot ItemSlot, UAA_Item* InItem)
{
	if (ItemSlot == EquipSlot)
	{
		RefreshItem();
	}
}

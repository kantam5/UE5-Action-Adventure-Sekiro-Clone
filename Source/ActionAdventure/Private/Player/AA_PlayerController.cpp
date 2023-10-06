// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AA_PlayerController.h"

#include "Characters/PlayerCharacter.h"
#include "UI/Player/AA_HUDWidget.h"
#include "UI/Player/ScreenOverlayWidget.h"
#include "UI/Player/ItemObtainedWidget.h"
#include "UI/Player/InventoryWidget.h"
#include "UI/Player/ConfirmWidget.h"
#include "UI/Player/HudItemSlot.h"
#include "UI/Player/IdolWidget.h"
#include "AbilitySystem/AttributeSets/AA_AttributeSetBase.h"
#include "Characters/BossCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "ActionAdventureGameMode.h"
#include "DataAssets/AA_Item.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameInstances/AA_GameInstance.h"
#include "GameInstances/SaveGameInstanceSubsystem.h"
#include "GameInstances/AA_SaveGame.h"

void AAA_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PlayerCharacter = Cast<APlayerCharacter>(InPawn);

	CreateHUD();

	if (!BossDefeatedWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Missing BossDefeatedWidgetClass"));
		return;
	}
	if (!DeathWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Missing DeathWidgetClass"));
		return;
	}
	BossDefeatedWidget = CreateWidget<UScreenOverlayWidget>(this, BossDefeatedWidgetClass);
	BossDefeatedWidget->AddToViewport();
	BossDefeatedWidget->SetRenderOpacity(0.f);

	DeathWidget = CreateWidget<UScreenOverlayWidget>(this, DeathWidgetClass);
	DeathWidget->AddToViewport();
	DeathWidget->SetRenderOpacity(0.f);

	IdolFoundWidget = CreateWidget<UScreenOverlayWidget>(this, IdolFoundWidgetClass);
	IdolFoundWidget->AddToViewport();
	IdolFoundWidget->SetRenderOpacity(0.f);

	ItemObtainedWidget = CreateWidget<UItemObtainedWidget>(this, ItemObtainedWidgetClass);

	InventoryWidget = CreateWidget<UInventoryWidget>(this, InventoryWidgetClass);

	ConfirmWidget = CreateWidget<UConfirmWidget>(this, ConfirmWidgetClass);

	IdolWidget = CreateWidget<UIdolWidget>(this, IdolWidgetClass);

}

void AAA_PlayerController::BeginPlay()
{
	LoadInventory();

	Super::BeginPlay();
}

void AAA_PlayerController::CreateHUD()
{
	if (UIHUDWidget)
	{
		return;
	}

	if (!UIHUDWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Missing UIHUDWidgetClass"));
		return;
	}

	UIHUDWidget = CreateWidget<UAA_HUDWidget>(this, UIHUDWidgetClass);
	UIHUDWidget->AddToViewport();

	UAA_AttributeSetBase* PlayerAttributeSet = PlayerCharacter.Get()->GetAttributeSet();

	if (UIHUDWidget->IsValidLowLevel())
	{
		UIHUDWidget->SetBossUIVisibility(false);
		UIHUDWidget->SetPlayerHealthPercentage(PlayerAttributeSet->GetHealth() / PlayerAttributeSet->GetMaxHealth());
		UIHUDWidget->SetPlayerStaminaPercentage(PlayerAttributeSet->GetStamina() / PlayerAttributeSet->GetMaxStamina());
		UIHUDWidget->SetPlayerMaxHealth(PlayerAttributeSet->GetMaxHealth());
		UIHUDWidget->SetPlayerMaxStamina(PlayerAttributeSet->GetMaxStamina());
	}
	
}

void AAA_PlayerController::SetBossHUD(ABossCharacter* Boss)
{
	UAA_AttributeSetBase* BossAttributeSet = Boss->GetAttributeSet();

	if (UIHUDWidget->IsValidLowLevel())
	{
		UIHUDWidget->SetBossUIVisibility(true);
		UIHUDWidget->SetBossHealthPercentage(BossAttributeSet->GetHealth() / BossAttributeSet->GetMaxHealth());
		UIHUDWidget->SetBossStaminaPercentage(BossAttributeSet->GetStamina() / BossAttributeSet->GetMaxStamina());
		UIHUDWidget->SetBossMaxHealth(BossAttributeSet->GetMaxHealth());
		UIHUDWidget->SetBossMaxStamina(BossAttributeSet->GetMaxStamina());
		UIHUDWidget->InitBossDeathCountNumber(BossAttributeSet->GetMaxDeathCount());
		UIHUDWidget->SetBossNameTextBlock(Boss->GetBossName());
		UIHUDWidget->SetBossUI(Boss);
	}

	Boss->OnCharacterDied.AddUniqueDynamic(this, &AAA_PlayerController::OnBossDied);
}

void AAA_PlayerController::SetBossUIVisibility(bool InState)
{
	UIHUDWidget->SetBossUIVisibility(InState);
}

void AAA_PlayerController::OnBossDied(AAA_CharacterBase* Boss)
{
	BossDefeatedWidget->PlayExecuteAnimation();

	if (BossEfeatedSound->IsValidLowLevel())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), BossEfeatedSound);
	}
}

void AAA_PlayerController::OnPlayerDied()
{
	DeathWidget->PlayExecuteAnimation();

	if (DeathWidget->IsValidLowLevel())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), DeathSound);
	}

	GetWorldTimerManager().SetTimer(GameOverHandle, FTimerDelegate::CreateLambda([this]()
		{
			SetBossUIVisibility(false);

			AActionAdventureGameMode* GameMode = Cast<AActionAdventureGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
			if (GameMode)
			{
				GameMode->GameOver();
				DeathWidget->SetRenderOpacity(0.f);
			}
		}), 5.5f, false);
}

void AAA_PlayerController::ShowItemPickupUI(UAA_Item* ItemType)
{
	SetPause(true);
	ItemObtainedWidget->AddToViewport();

	SetShowMouseCursor(true);
	SetInputMode(FInputModeGameAndUI());
	ItemObtainedWidget->ShowObtainedItemUI(ItemType);

	if (ItemObtainedSound->IsValidLowLevel())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ItemObtainedSound);
	}
	
}

void AAA_PlayerController::EndItemPickupUI()
{
	SetPause(false);
	SetShowMouseCursor(false);
	SetInputMode(FInputModeGameOnly());
}

void AAA_PlayerController::ShowInventoryUI()
{
	PlayerCharacter.Get()->SetInventoryUsing(true);
	SetPause(true);
	SetShowMouseCursor(true);
	InventoryWidget->AddToViewport();
	SetInputMode(FInputModeGameAndUI());
}

void AAA_PlayerController::EndInventoryUI()
{
	PlayerCharacter.Get()->SetInventoryUsing(false);
	SetPause(false);
	SetShowMouseCursor(false);
	InventoryWidget->RemoveFromParent();
	SetInputMode(FInputModeGameOnly());
}

void AAA_PlayerController::ShowConfirmUI(UAA_Item* InItem)
{
	SelectedItem = InItem;

	if (ConfirmWidget && SelectedItem)
	{
		ConfirmWidget->Button_OK->OnClicked.Clear();
		ConfirmWidget->Button_Cancel->OnClicked.Clear();
		ConfirmWidget->AddToViewport();
		FString UseText = FString::Printf(TEXT("Use %s"), *SelectedItem->ItemName.ToString());
		ConfirmWidget->TextBlock_Confirm->SetText(FText::FromString(UseText));
		ConfirmWidget->Button_OK->OnClicked.AddDynamic(this, &AAA_PlayerController::UseSelectedItem);
		ConfirmWidget->Button_Cancel->OnClicked.AddDynamic(this, &AAA_PlayerController::EndConfirmUI);
	}
}

void AAA_PlayerController::UseSelectedItem()
{
	ConfirmWidget->RemoveFromParent();
	EndInventoryUI();
	
	PlayerCharacter.Get()->ActiveAbilityWithItem(SelectedItem);

	if (SelectedItem)
	{
		SelectedItem = nullptr;
	}
}

void AAA_PlayerController::EndConfirmUI()
{
	ConfirmWidget->RemoveFromParent();
	if (SelectedItem)
	{
		SelectedItem = nullptr;
	}
}

void AAA_PlayerController::UpdateHUDItemSlot()
{
	UIHUDWidget->WBP_HudItemSlot_Skill->UpdateSlot(PlayerCharacter->CurrentSkillIndex);
	UIHUDWidget->WBP_HudItemSlot_Potion->UpdateSlot(PlayerCharacter->CurrentPotionIndex);
}

void AAA_PlayerController::OnIdolFound()
{
	IdolFoundWidget->PlayExecuteAnimation();

	UGameplayStatics::PlaySound2D(GetWorld(), IdolFoundSound);
}

void AAA_PlayerController::ShowIdolUI(ACheckPoint* InIdol)
{
	PlayerCharacter.Get()->SetInventoryUsing(true);
	SetShowMouseCursor(true);
	IdolWidget->AddToViewport();
	IdolWidget->SetIdolData(InIdol);
	SetInputMode(FInputModeUIOnly());
}

void AAA_PlayerController::EndIdolUI()
{
	PlayerCharacter.Get()->SetInventoryUsing(false);
	SetPause(false);
	SetShowMouseCursor(false);
	IdolWidget->RemoveFromParent();
	SetInputMode(FInputModeGameOnly());
}

/*------------------------------------------Inventory-------------------------------------------*/

void AAA_PlayerController::InventoryItemChanged(bool bAdded, UAA_Item* Item)
{
	UpdateHUDItemSlot();
}

void AAA_PlayerController::SlottedItemChanged(FAA_ItemSlot ItemSlot, UAA_Item* Item)
{
	UpdateHUDItemSlot();
}

bool AAA_PlayerController::AddInventoryItem(UAA_Item* NewItem, int32 ItemCount, bool bAutoSlot)
{
	bool bChanged = false;
	if (!NewItem)
	{
		return false;
	}

	if (ItemCount <= 0)
	{
		return false;
	}

	FAA_ItemData OldData;
	GetInventoryItemData(NewItem, OldData);

	FAA_ItemData NewData = OldData;
	NewData.UpdateItemData(FAA_ItemData(ItemCount), NewItem->MaxCount);

	if (OldData != NewData)
	{
		InventoryData.Add(NewItem, NewData);
		NotifyInventoryItemChanged(true, NewItem);
		bChanged = true;
	}

	if (bAutoSlot)
	{
		bChanged |= FillEmptySlotWithItem(NewItem);
	}

	if (bChanged)
	{
		SaveInventory();
		return true;
	}

	return false;

}

bool AAA_PlayerController::RemoveInventoryItem(UAA_Item* RemovedItem, int32 RemoveCount)
{
	if (!RemovedItem)
	{
		return false;
	}

	FAA_ItemData NewData;
	GetInventoryItemData(RemovedItem, NewData);

	if (!NewData.IsValid())
	{
		return false;
	}

	// RemoveCount가 0이하라면 모두 삭제한다.
	if (RemoveCount <= 0)
	{
		NewData.ItemCount = 0;
	}
	else
	{
		NewData.ItemCount -= RemoveCount;
	}

	if (NewData.ItemCount > 0)
	{
		InventoryData.Add(RemovedItem, NewData);
	}
	else
	{
		// 모두 지웠을 경우, unslotted
		InventoryData.Remove(RemovedItem);

		for (TPair<FAA_ItemSlot, UAA_Item*>& Pair : SlottedItems)
		{
			if (Pair.Value == RemovedItem)
			{
				Pair.Value = nullptr;
				NotifySlottedItemChanged(Pair.Key, Pair.Value);
			}
		}
	}

	NotifyInventoryItemChanged(false, RemovedItem);

	SaveInventory();
	return true;
}

void AAA_PlayerController::GetInventoryItems(TArray<UAA_Item*>& Items, FPrimaryAssetType ItemType)
{
	for (const TPair<UAA_Item*, FAA_ItemData>& Pair : InventoryData)
	{
		if (Pair.Key)
		{
			FPrimaryAssetId AssetId = Pair.Key->GetPrimaryAssetId();

			if (AssetId.PrimaryAssetType == ItemType || !ItemType.IsValid())
			{
				Items.Add(Pair.Key);
			}
		}
	}
}

int32 AAA_PlayerController::GetInventoryItemCount(UAA_Item* Item) const
{
	const FAA_ItemData* FoundItem = InventoryData.Find(Item);
	if (FoundItem)
	{
		return FoundItem->ItemCount;
	}

	return 0;
}

bool AAA_PlayerController::GetInventoryItemData(UAA_Item* Item, FAA_ItemData& ItemData) const
{
	const FAA_ItemData* FoundItem = InventoryData.Find(Item);
	if (FoundItem)
	{
		ItemData = *FoundItem;
		return true;
	}

	ItemData = FAA_ItemData(0);
	return false;
}

bool AAA_PlayerController::SetSlottedItem(FAA_ItemSlot ItemSlot, UAA_Item* Item)
{
	bool bFound = false;
	for (TPair<FAA_ItemSlot, UAA_Item*>& Pair : SlottedItems)
	{
		if (Pair.Key == ItemSlot)
		{
			bFound = true;
			Pair.Value = Item;
			NotifySlottedItemChanged(Pair.Key, Pair.Value);
		}
		else if (Item != nullptr && Pair.Value == Item)
		{
			Pair.Value = nullptr;
			NotifySlottedItemChanged(Pair.Key, Pair.Value);
		}
	}

	if (bFound)
	{
		SaveInventory();
		return true;
	}

	return false;
}

UAA_Item* AAA_PlayerController::GetSlottedItem(FAA_ItemSlot ItemSlot) const
{
	UAA_Item* const* FoundItem = SlottedItems.Find(ItemSlot);
	if (FoundItem)
	{
		return *FoundItem;
	}

	return nullptr;
}

void AAA_PlayerController::GetSlottedItems(TArray<UAA_Item*>& Items, FPrimaryAssetType ItemType, bool bOutputEmptyIndexes)
{
	for (TPair<FAA_ItemSlot, UAA_Item*>& Pair : SlottedItems)
	{
		if (Pair.Key.ItemType == ItemType || !ItemType.IsValid())
		{
			Items.Add(Pair.Value);
		}
	}
}

void AAA_PlayerController::FillEmptySlots()
{
	bool bShouldSave = false;
	for (const TPair<UAA_Item*, FAA_ItemData>& Pair : InventoryData)
	{
		bShouldSave |= FillEmptySlotWithItem(Pair.Key);
	}
	if (bShouldSave)
	{
		SaveInventory();
	}
}

bool AAA_PlayerController::SaveInventory()
{
	USaveGameInstanceSubsystem* SG = GetGameInstance()->GetSubsystem<USaveGameInstanceSubsystem>();
	if (!SG)
	{
		return false;
	}

	UAA_SaveGame* CurrentSaveGame = SG->GetCurrentSaveGame();
	if (CurrentSaveGame)
	{
		CurrentSaveGame->InventoryData.Reset();
		CurrentSaveGame->SlottedItems.Reset();

		for (const TPair<UAA_Item*, FAA_ItemData>& ItemPair : InventoryData)
		{
			FPrimaryAssetId AssetId;

			if (ItemPair.Key)
			{
				AssetId = ItemPair.Key->GetPrimaryAssetId();
				CurrentSaveGame->InventoryData.Add(AssetId, ItemPair.Value);
			}
		}

		for (const TPair<FAA_ItemSlot, UAA_Item*>& SlotPair : SlottedItems)
		{
			FPrimaryAssetId AssetId;

			if (SlotPair.Value)
			{
				AssetId = SlotPair.Value->GetPrimaryAssetId();
			}
		}

		return true;
	}

	return false;
}

bool AAA_PlayerController::LoadInventory()
{
	InventoryData.Reset();
	SlottedItems.Reset();

	UWorld* World = GetWorld();
	UAA_GameInstance* GameInstance = World ? World->GetGameInstance<UAA_GameInstance>() : nullptr;
	if (!GameInstance)
	{
		return false;
	}

	USaveGameInstanceSubsystem* SG = GetGameInstance()->GetSubsystem<USaveGameInstanceSubsystem>();
	if (!SG)
	{
		return false;
	}

	for (const TPair<FPrimaryAssetType, int32>& Pair : GameInstance->ItemSlotsPerType)
	{
		for (int32 SlotNumber = 0; SlotNumber < Pair.Value; SlotNumber++)
		{
			SlottedItems.Add(FAA_ItemSlot(Pair.Key, SlotNumber), nullptr);
		}
	}

	UAA_SaveGame* CurrentSaveGame = SG->GetCurrentSaveGame();
	UAA_AssetManager& AssetManager = UAA_AssetManager::Get();
	if (CurrentSaveGame)
	{
		bool bFoundAnySlots = false;
		for (const TPair<FPrimaryAssetId, FAA_ItemData>& ItemPair : CurrentSaveGame->InventoryData)
		{
			UAA_Item* LoadedItem = AssetManager.ForceLoadItem(ItemPair.Key);
			if (LoadedItem)
			{
				InventoryData.Add(LoadedItem, ItemPair.Value);
			}
		}

		for (const TPair<FAA_ItemSlot, FPrimaryAssetId>& SlotPair : CurrentSaveGame->SlottedItems)
		{
			if (SlotPair.Value.IsValid())
			{
				UAA_Item* LoadedItem = AssetManager.ForceLoadItem(SlotPair.Value);
				if (GameInstance->IsValidItemSlot(SlotPair.Key) && LoadedItem)
				{
					SlottedItems.Add(SlotPair.Key, LoadedItem);
					bFoundAnySlots = true;
				}
			}
		}

		if (!bFoundAnySlots)
		{
			FillEmptySlots();
		}

		NotifyInventoryLoaded();

		return true;
	}

	NotifyInventoryLoaded();

	return false;
}

bool AAA_PlayerController::FillEmptySlotWithItem(UAA_Item* NewItem)
{
	FPrimaryAssetType NewItemType = NewItem->GetPrimaryAssetId().PrimaryAssetType;
	FAA_ItemSlot EmptySlot;
	for (TPair<FAA_ItemSlot, UAA_Item*>& Pair : SlottedItems)
	{
		if (Pair.Key.ItemType == NewItemType)
		{
			if (Pair.Value == NewItem)
			{
				// 이미 있는 아이템이라면
				return false;
			}
			else if (Pair.Value == nullptr && (!EmptySlot.IsValid() || EmptySlot.SlotNumber > Pair.Key.SlotNumber))
			{
				// 채울 수 있는 비어있는 슬롯을 찾은 경우
				EmptySlot = Pair.Key;
			}
		}
	}

	if (EmptySlot.IsValid())
	{
		SlottedItems[EmptySlot] = NewItem;
		NotifySlottedItemChanged(EmptySlot, NewItem);
		return true;
	}

	return false;
}

void AAA_PlayerController::NotifyInventoryItemChanged(bool bAdded, UAA_Item* Item)
{
	OnInventoryItemChangedNative.Broadcast(bAdded, Item);
	OnInventoryItemChanged.Broadcast(bAdded, Item);

	InventoryItemChanged(bAdded, Item);
}

void AAA_PlayerController::NotifySlottedItemChanged(FAA_ItemSlot ItemSlot, UAA_Item* Item)
{
	OnSlottedItemChangedNative.Broadcast(ItemSlot, Item);
	OnSlottedItemChanged.Broadcast(ItemSlot, Item);

	SlottedItemChanged(ItemSlot, Item);
}

void AAA_PlayerController::NotifyInventoryLoaded()
{
	OnInventoryLoadedNative.Broadcast();
	OnInventoryLoaded.Broadcast();
}

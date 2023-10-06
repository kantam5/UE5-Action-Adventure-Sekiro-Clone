// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/AA_InventoryInterface.h"
#include "AA_GameTypes.h"
#include "AA_PlayerController.generated.h"

class APlayerCharacter;
class UAA_HUDWidget;
class AAA_CharacterBase;
class ABossCharacter;
class UScreenOverlayWidget;
class UItemObtainedWidget;
class UInventoryWidget;
class UConfirmWidget;
class UIdolWidget;
class ACheckPoint;

UCLASS()
class ACTIONADVENTURE_API AAA_PlayerController : public APlayerController, public IAA_InventoryInterface
{
	GENERATED_BODY()

public:
	void CreateHUD();

	void SetBossHUD(ABossCharacter* Boss);

	void SetBossUIVisibility(bool InState);

	UFUNCTION()
	void OnBossDied(AAA_CharacterBase* Boss);

	void OnPlayerDied();

	void ShowItemPickupUI(UAA_Item* ItemType);
	void EndItemPickupUI();

	void ShowInventoryUI();
	void EndInventoryUI();

	void ShowConfirmUI(UAA_Item* InItem);
	UFUNCTION()
	void UseSelectedItem();
	UFUNCTION()
	void EndConfirmUI();

	void UpdateHUDItemSlot();

	void OnIdolFound();

	void ShowIdolUI(ACheckPoint* InIdol);
	void EndIdolUI();

	FORCEINLINE UAA_HUDWidget* GetHUD() { return UIHUDWidget; }
	FORCEINLINE UInventoryWidget* GetInventoryWidget() { return InventoryWidget; }

protected:
	TWeakObjectPtr<APlayerCharacter> PlayerCharacter;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAA_HUDWidget> UIHUDWidgetClass;
	UPROPERTY()
	UAA_HUDWidget* UIHUDWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UScreenOverlayWidget> BossDefeatedWidgetClass;
	UPROPERTY()
	UScreenOverlayWidget* BossDefeatedWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UScreenOverlayWidget> DeathWidgetClass;
	UPROPERTY()
	UScreenOverlayWidget* DeathWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UScreenOverlayWidget> IdolFoundWidgetClass;
	UPROPERTY()
	UScreenOverlayWidget* IdolFoundWidget;

	UPROPERTY(EditDefaultsOnly)
	USoundBase* BossEfeatedSound;

	UPROPERTY(EditDefaultsOnly)
	USoundBase* DeathSound;

	UPROPERTY(EditDefaultsOnly)
	USoundBase* IdolFoundSound;

	FTimerHandle GameOverHandle;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UItemObtainedWidget> ItemObtainedWidgetClass;
	UPROPERTY()
	UItemObtainedWidget* ItemObtainedWidget;

	UPROPERTY(EditDefaultsOnly)
	USoundBase* ItemObtainedSound;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;
	UPROPERTY()
	UInventoryWidget* InventoryWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UConfirmWidget> ConfirmWidgetClass;
	UPROPERTY()
	UConfirmWidget* ConfirmWidget;
	UPROPERTY()
	UAA_Item* SelectedItem;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UIdolWidget> IdolWidgetClass;
	UPROPERTY()
	UIdolWidget* IdolWidget;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;


/*-------------------------Inventory------------------------*/
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TMap<UAA_Item*, FAA_ItemData> InventoryData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TMap<FAA_ItemSlot, UAA_Item*> SlottedItems;

	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnInventoryItemChanged OnInventoryItemChanged;
	FOnInventoryItemChangedNative OnInventoryItemChangedNative;

	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnSlottedItemChanged OnSlottedItemChanged;
	FOnSlottedItemChangedNative OnSlottedItemChangedNative;

	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnInventoryLoaded OnInventoryLoaded;
	FOnInventoryLoadedNative OnInventoryLoadedNative;

	UFUNCTION(Category = Inventory)
	void InventoryItemChanged(bool bAdded, UAA_Item* Item);

	UFUNCTION(Category = Inventory)
	void SlottedItemChanged(FAA_ItemSlot ItemSlot, UAA_Item* Item);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool AddInventoryItem(UAA_Item* NewItem, int32 ItemCount = 1, bool bAutoSlot = true);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool RemoveInventoryItem(UAA_Item* RemovedItem, int32 RemoveCount = 1);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void GetInventoryItems(TArray<UAA_Item*>& Items, FPrimaryAssetType ItemType);

	UFUNCTION(BlueprintPure, Category = Inventory)
	int32 GetInventoryItemCount(UAA_Item* Item) const;

	UFUNCTION(BlueprintPure, Category = Inventory)
	bool GetInventoryItemData(UAA_Item* Item, FAA_ItemData& ItemData) const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool SetSlottedItem(FAA_ItemSlot ItemSlot, UAA_Item* Item);

	UFUNCTION(BlueprintPure, Category = Inventory)
	UAA_Item* GetSlottedItem(FAA_ItemSlot ItemSlot) const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void GetSlottedItems(TArray<UAA_Item*>& Items, FPrimaryAssetType ItemType, bool bOutputEmptyIndexes);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void FillEmptySlots();

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool SaveInventory();

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool LoadInventory();

	virtual const TMap<UAA_Item*, FAA_ItemData>& GetInventoryDataMap() const override
	{
		return InventoryData;
	}
	virtual const TMap<FAA_ItemSlot, UAA_Item*>& GetSlottedItemMap() const override
	{
		return SlottedItems;
	}
	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate() override
	{
		return OnInventoryItemChangedNative;
	}
	virtual FOnSlottedItemChangedNative& GetSlottedItemChangedDelegate() override
	{
		return OnSlottedItemChangedNative;
	}

protected:
	bool FillEmptySlotWithItem(UAA_Item* NewItem);

	void NotifyInventoryItemChanged(bool bAdded, UAA_Item* Item);
	void NotifySlottedItemChanged(FAA_ItemSlot ItemSlot, UAA_Item* Item);
	void NotifyInventoryLoaded();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryGrid.generated.h"

class UUniformGridPanel;
class UInventorySlot;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UInventoryGrid : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void AddSlots();

	UFUNCTION()
	void OnInventoryItemChanged(bool bAdded, UAA_Item* Item);
	
protected:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	UUniformGridPanel* UniformGridPanel_ItemGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FPrimaryAssetType ItemType;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventorySlot> SlotClass;

	int32 ItemRow;

};

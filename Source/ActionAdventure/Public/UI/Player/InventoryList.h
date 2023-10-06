// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryList.generated.h"

class UListView;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UInventoryList : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UListView* ListView_ItemList;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	TArray<FPrimaryAssetType> ItemTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FPrimaryAssetType ItemType;

public:
	virtual void NativeConstruct() override;

	void AddItems();

	UFUNCTION()
	void OnInventoryItemChanged(bool bAdded, UAA_Item* Item);

	UFUNCTION(BlueprintCallable)
	void OnItemHovered(UObject* InItem, bool bIsHovered);
};

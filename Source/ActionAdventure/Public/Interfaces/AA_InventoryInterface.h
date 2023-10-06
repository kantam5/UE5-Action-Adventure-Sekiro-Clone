// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AA_GameTypes.h"
#include "AA_InventoryInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAA_InventoryInterface : public UInterface
{
	GENERATED_BODY()
};

class ACTIONADVENTURE_API IAA_InventoryInterface
{
	GENERATED_BODY()

public:
	virtual const TMap<UAA_Item*, FAA_ItemData>& GetInventoryDataMap() const = 0;

	virtual const TMap<FAA_ItemSlot, UAA_Item*>& GetSlottedItemMap() const = 0;

	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate() = 0;

	virtual FOnSlottedItemChangedNative& GetSlottedItemChangedDelegate() = 0;

	// TODO: SaveGame
};

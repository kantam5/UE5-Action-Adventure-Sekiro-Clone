// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AA_GameTypes.generated.h"

class UAA_Item;

UENUM(BlueprintType)
enum class EHitReactDirection : uint8
{
	None		UMETA(DisplayName = "None"),
	Left		UMETA(DisplayName = "Left"),
	Front		UMETA(DisplayName = "Front"),
	Right		UMETA(DisplayName = "Right"),
	Back		UMETA(DisplayName = "Back")
};

USTRUCT(BlueprintType)
struct ACTIONADVENTURE_API FAA_ItemSlot
{
	GENERATED_BODY()

	FAA_ItemSlot()
		: SlotNumber(-1)
	{}

	FAA_ItemSlot(const FPrimaryAssetType& InItemType, int32 InSlotNumber)
		: ItemType(InItemType)
		, SlotNumber(InSlotNumber)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FPrimaryAssetType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 SlotNumber;

	bool operator==(const FAA_ItemSlot& Other) const
	{
		return ItemType == Other.ItemType && SlotNumber == Other.SlotNumber;
	}
	bool operator!=(const FAA_ItemSlot& Other) const
	{
		return !(*this == Other);
	}

	friend inline uint32 GetTypeHash(const FAA_ItemSlot& Key)
	{
		int32 Hash = 0;

		Hash = HashCombine(Hash, GetTypeHash(Key.ItemType));
		Hash = HashCombine(Hash, (uint32)Key.SlotNumber);
		return Hash;
	}

	bool IsValid() const
	{
		return ItemType.IsValid() && SlotNumber >= 0;
	}
};

USTRUCT(BlueprintType)
struct ACTIONADVENTURE_API FAA_ItemData
{
	GENERATED_BODY()

	FAA_ItemData()
		: ItemCount(1)
	{}

	FAA_ItemData(int32 InItemCount)
		: ItemCount(InItemCount)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 ItemCount;

	bool operator==(const FAA_ItemData& Other) const
	{
		return ItemCount == Other.ItemCount;
	}
	bool operator!=(const FAA_ItemData& Other) const
	{
		return !(*this == Other);
	}

	bool IsValid() const
	{
		return ItemCount > 0;
	}

	void UpdateItemData(const FAA_ItemData& Other, int32 MaxCount)
	{
		if (MaxCount <= 0)
		{
			MaxCount = MAX_int32;
		}

		ItemCount = FMath::Clamp(ItemCount + Other.ItemCount, 1, MaxCount);
	}
};

UENUM(BlueprintType)
enum class EFoot : uint8
{
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChanged, bool, bAdded, UAA_Item*, Item);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChangedNative, bool, UAA_Item*);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlottedItemChanged, FAA_ItemSlot, ItemSlot, UAA_Item*, Item);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSlottedItemChangedNative, FAA_ItemSlot, UAA_Item*);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryLoaded);
DECLARE_MULTICAST_DELEGATE(FOnInventoryLoadedNative);
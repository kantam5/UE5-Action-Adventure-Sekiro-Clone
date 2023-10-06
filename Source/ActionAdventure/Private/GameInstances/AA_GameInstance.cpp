// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstances/AA_GameInstance.h"

#include "AbilitySystemGlobals.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

UAA_GameInstance::UAA_GameInstance()
{
}

void UAA_GameInstance::Init()
{
	Super::Init();

	UAbilitySystemGlobals::Get().InitGlobalData();
}

void UAA_GameInstance::OnItemLoaded(FPrimaryAssetId ItemId)
{
	
}

bool UAA_GameInstance::IsValidItemSlot(FAA_ItemSlot ItemSlot) const
{
	if (ItemSlot.IsValid())
	{
		const int32* FoundCount = ItemSlotsPerType.Find(ItemSlot.ItemType);
		if (FoundCount)
		{
			return ItemSlot.SlotNumber < *FoundCount;
		}
	}

	return false;
}

void UAA_GameInstance::InitializeStoreItems()
{
	if (UAssetManager* Manager = UAssetManager::GetIfValid())
	{
		for (auto ItemSlots : ItemSlotsPerType)
		{
			TArray<FPrimaryAssetId> AssetIdList;
			UKismetSystemLibrary::GetPrimaryAssetIdList(ItemSlots.Key, AssetIdList);
			for (FPrimaryAssetId AssetId : AssetIdList)
			{
				FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &UAA_GameInstance::OnItemLoaded, AssetId);
			}


		}
	}
	
}

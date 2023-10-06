// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/AA_Item.h"

UAA_Item::UAA_Item()
	: MaxCount(1)
{
}

bool UAA_Item::IsConsumable() const
{
	if (MaxCount <= 0)
	{
		return true;
	}
	return false;
}

FString UAA_Item::GetIdentifierString() const
{
	return GetPrimaryAssetId().ToString();
}

FPrimaryAssetId UAA_Item::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(ItemType, GetFName());
}

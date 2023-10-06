// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/AA_Item.h"
#include "AA_PotionItem.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UAA_PotionItem : public UAA_Item
{
	GENERATED_BODY()

public:
	UAA_PotionItem()
	{
		ItemType = UAA_AssetManager::PotionItemType;
	}
	
};

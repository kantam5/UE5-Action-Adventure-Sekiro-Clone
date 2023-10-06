// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/AA_Item.h"
#include "AA_CombatArt.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UAA_CombatArt : public UAA_Item
{
	GENERATED_BODY()
	
public:
	UAA_CombatArt()
	{
		ItemType = UAA_AssetManager::CombatArtItemType;
	}
};

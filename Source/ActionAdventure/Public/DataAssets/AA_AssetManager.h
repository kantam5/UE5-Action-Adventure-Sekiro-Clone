// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AA_AssetManager.generated.h"

class UAA_Item;

UCLASS()
class ACTIONADVENTURE_API UAA_AssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	UAA_AssetManager() {}
	virtual void StartInitialLoading() override;

	static const FPrimaryAssetType PotionItemType;
	static const FPrimaryAssetType SkillItemType;
	static const FPrimaryAssetType CombatArtItemType;
	static const FPrimaryAssetType KeyItemType;

	static UAA_AssetManager& Get();

	UAA_Item* ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning = true);
};

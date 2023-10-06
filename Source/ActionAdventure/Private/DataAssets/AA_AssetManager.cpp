// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/AA_AssetManager.h"
#include "DataAssets/AA_Item.h"

const FPrimaryAssetType UAA_AssetManager::PotionItemType = TEXT("Potion");
const FPrimaryAssetType UAA_AssetManager::SkillItemType = TEXT("Skill");
const FPrimaryAssetType UAA_AssetManager::CombatArtItemType = TEXT("CombatArt");
const FPrimaryAssetType UAA_AssetManager::KeyItemType = TEXT("Key");

void UAA_AssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();


}

UAA_AssetManager& UAA_AssetManager::Get()
{
	UAA_AssetManager* This = Cast<UAA_AssetManager>(GEngine->AssetManager);

	if (This)
	{
		return *This;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini"));
		return *NewObject<UAA_AssetManager>();
	}
}

UAA_Item* UAA_AssetManager::ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning)
{
	FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

	UAA_Item* LoadedItem = Cast<UAA_Item>(ItemPath.TryLoad());

	if (bLogWarning && LoadedItem == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load item for identifier &s!"), *PrimaryAssetId.ToString());
	}

	return LoadedItem;
}

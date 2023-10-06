// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAssets/AA_AssetManager.h"
#include "AA_Item.generated.h"

class UAA_GameplayAbility;

UCLASS()
class ACTIONADVENTURE_API UAA_Item : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UAA_Item();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
	FPrimaryAssetType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FSlateBrush ItemIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	int32 MaxCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	int32 ManaCost;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Max)
	bool IsConsumable() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	TSubclassOf<UAA_GameplayAbility> GrangedAbility;

	UFUNCTION(BlueprintCallable, Category = Item)
	FString GetIdentifierString() const;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};

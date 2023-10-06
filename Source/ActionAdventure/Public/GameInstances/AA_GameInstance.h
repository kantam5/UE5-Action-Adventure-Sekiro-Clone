// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AA_GameTypes.h"
#include "AA_GameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRestInCheckPointDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCheckPointAddedDelegate, ACheckPoint*, Idol);

class ACheckPoint;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UAA_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UAA_GameInstance();

	virtual void Init() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
	TMap<FPrimaryAssetId, FAA_ItemData> DefaultInventory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
	TMap<FPrimaryAssetType, int32> ItemSlotsPerType;

	UPROPERTY(BlueprintReadWrite, Category = Save)
	FString SaveSlot;

	UPROPERTY(BlueprintReadWrite, Category = Save)
	int32 SaveUserIndex;

	UPROPERTY(EditAnywhere, Category = CheckPoint)
	TArray<ACheckPoint*> CheckPoints;

	UPROPERTY()
	ACheckPoint* LatestCheckPoint;

	FRestInCheckPointDelegate OnRestInCheckPoint;

	FCheckPointAddedDelegate OnCheckPointAdded;

	void OnItemLoaded(FPrimaryAssetId ItemId);

	bool IsValidItemSlot(FAA_ItemSlot ItemSlot) const;

protected:
	UPROPERTY()
	TArray<UAA_Item*> StoreItems;

	void InitializeStoreItems();
};

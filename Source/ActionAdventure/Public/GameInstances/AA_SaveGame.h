// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AA_GameTypes.h"
#include "AA_SaveGame.generated.h"

class AItemPickupActor;
class ACheckPoint;

USTRUCT()
struct FActorSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName ActorName;

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	TArray<uint8> ByteData;
};

UCLASS()
class ACTIONADVENTURE_API UAA_SaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	TArray<FActorSaveData> SavedActors;

	UPROPERTY()
	FActorSaveData SavedPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FPrimaryAssetId, FAA_ItemData> InventoryData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FAA_ItemSlot, FPrimaryAssetId> SlottedItems;

	UPROPERTY(VisibleAnywhere)
	TMap<FName, AItemPickupActor*> SavedItems;

	UPROPERTY(VisibleAnywhere)
	TMap<FName, ACheckPoint*> SavedIdols;
};

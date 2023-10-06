// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/InteractionActorBase.h"
#include "ItemPickupActor.generated.h"

class UAA_Item;
class USphereComponent;

UCLASS()
class ACTIONADVENTURE_API AItemPickupActor : public AInteractionActorBase
{
	GENERATED_BODY()

public:
	AItemPickupActor();

	virtual bool OnInteract(AActor* Caller) override;

	FORCEINLINE void SetItemType(UAA_Item* InItemType) { ItemType = InItemType; }
	FORCEINLINE void SetDropped(bool InState) { bDropped = InState; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAA_Item* ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemCount;

	UPROPERTY()
	bool bDropped;

	void GiveItem();

	void SavePickedUp();

	virtual void OnActorLoaded_Implementation() override;
};

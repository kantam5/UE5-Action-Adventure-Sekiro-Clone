// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemPickupActor.h"

#include "Kismet/GameplayStatics.h"
#include "Player/AA_PlayerController.h"
#include "GameInstances/SaveGameInstanceSubsystem.h"
#include "GameInstances/AA_SaveGame.h"

AItemPickupActor::AItemPickupActor()
	: ItemCount(1)
	, bDropped(false)
{
}

bool AItemPickupActor::OnInteract(AActor* Caller)
{
	if (!Super::OnInteract(Caller))
	{
		return false;
	}

	SavePickedUp();

	GiveItem();

	return true;
}

void AItemPickupActor::GiveItem()
{
	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController)
	{
		PlayerController->ShowItemPickupUI(ItemType);
		PlayerController->AddInventoryItem(ItemType, ItemCount, false);

		Destroy();
	}
}

void AItemPickupActor::SavePickedUp()
{
	if (!bDropped)
	{
		USaveGameInstanceSubsystem* SG = GetGameInstance()->GetSubsystem<USaveGameInstanceSubsystem>();
		if (!SG)
		{
			return;
		}

		// write ÇÊ¿ä
		UAA_SaveGame* CurrentSaveGame = SG->GetCurrentSaveGame();
		if (CurrentSaveGame)
		{
			CurrentSaveGame->SavedItems.Add(GetFName(), this);

			UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SG->GetCurrentSlotName(), 0);
		}
	}
	
}

void AItemPickupActor::OnActorLoaded_Implementation()
{
	if (!bDropped)
	{
		USaveGameInstanceSubsystem* SG = GetGameInstance()->GetSubsystem<USaveGameInstanceSubsystem>();
		if (!SG)
		{
			return;
		}

		UAA_SaveGame* CurrentSaveGame = SG->GetCurrentSaveGame();
		if (CurrentSaveGame)
		{
			if (CurrentSaveGame->SavedItems.Find(GetFName()) != nullptr)
			{
				Destroy();
			}
		}
	}
}

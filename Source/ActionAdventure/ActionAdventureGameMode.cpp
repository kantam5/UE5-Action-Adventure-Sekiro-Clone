// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActionAdventureGameMode.h"

#include "ActionAdventureCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstances/AA_GameInstance.h"
#include "Actors/CheckPoint.h"
#include "GameInstances/SaveGameInstanceSubsystem.h"
#include "Kismet/GameplayStatics.h"

AActionAdventureGameMode::AActionAdventureGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Core/Blueprints/Characters/BP_PlayerCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AActionAdventureGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	USaveGameInstanceSubsystem* SG = GetGameInstance()->GetSubsystem<USaveGameInstanceSubsystem>();

	FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");
	SG->LoadSaveGame(SelectedSaveSlot);
}

void AActionAdventureGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	USaveGameInstanceSubsystem* SG = GetGameInstance()->GetSubsystem<USaveGameInstanceSubsystem>();
	SG->OverrideSpawnTransform(NewPlayer);
}

void AActionAdventureGameMode::ResetLevel()
{
	Super::ResetLevel();
}

void AActionAdventureGameMode::GameOver()
{
	// 죽었을 때 호출, 레벨을 다시 시작
	UAA_GameInstance* GameInstance = Cast<UAA_GameInstance>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->LatestCheckPoint)
		{
			GameInstance->LatestCheckPoint->Rest();
			GameInstance->LatestCheckPoint->Travel();
		}
		else
		{
			UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
		}
	}
}

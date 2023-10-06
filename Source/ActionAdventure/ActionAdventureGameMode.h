// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ActionAdventureGameMode.generated.h"

UCLASS(minimalapi)
class AActionAdventureGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AActionAdventureGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	virtual void ResetLevel() override;

	virtual void GameOver();
};




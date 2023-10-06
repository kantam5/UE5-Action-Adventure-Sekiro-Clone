// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveGameInstanceSubsystem.generated.h"

class UAA_SaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameSignature, class UAA_SaveGame*, SaveObject);

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API USaveGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
protected:
	FString CurrentSlotName;

	UPROPERTY()
	TObjectPtr<UAA_SaveGame> CurrentSaveGame;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	void SetSlotName(FString NewSlotName);

	UFUNCTION(BlueprintCallable)
	void WriteSaveGame();

	UFUNCTION(BlueprintCallable)
	void LoadSaveGame(FString InSlotName = "");

	UFUNCTION(BlueprintCallable)
	bool OverrideSpawnTransform(AController* NewPlayer);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UAA_SaveGame* GetCurrentSaveGame() { return CurrentSaveGame; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FString GetCurrentSlotName() { return CurrentSlotName; }

	UPROPERTY(BlueprintAssignable)
	FOnSaveGameSignature OnSaveGameWritten;

	UPROPERTY(BlueprintAssignable)
	FOnSaveGameSignature OnSaveGameLoaded;
};

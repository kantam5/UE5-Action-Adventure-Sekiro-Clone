// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstances/SaveGameInstanceSubsystem.h"

#include "EngineUtils.h"
#include "GameInstances/AA_SaveGame.h"
#include "Interfaces/SaveGameInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Kismet/GameplayStatics.h"
#include "DeveloperSettings/SaveGameSettings.h"
#include "GameFramework/Character.h"

void USaveGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const USaveGameSettings* SGSettings = GetDefault<USaveGameSettings>();
	CurrentSlotName = SGSettings->SaveSlotName;
}

void USaveGameInstanceSubsystem::SetSlotName(FString NewSlotName)
{
	if (NewSlotName.Len() == 0)
	{
		return;
	}

	CurrentSlotName = NewSlotName;
}

void USaveGameInstanceSubsystem::WriteSaveGame()
{
	CurrentSaveGame.Get()->SavedActors.Empty();
	CurrentSaveGame.Get()->SavedPlayer = { FName(""), FTransform(), TArray<uint8>() };

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerCharacter)
	{
		FActorSaveData ActorData;
		ActorData.ActorName = PlayerCharacter->GetFName();
		ActorData.Transform = PlayerCharacter->GetActorTransform();

		FMemoryWriter MemWriter(ActorData.ByteData);

		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);

		Ar.SetIsSaving(true);

		PlayerCharacter->Serialize(Ar);

		CurrentSaveGame->SavedPlayer = ActorData;
	}

	// World의 Actor들을 Array에 저장
	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		// USaveGameInterface를 상속받은 Actor만 저장
		if (!IsValid(Actor) || !Actor->Implements<USaveGameInterface>())
		{
			continue;
		}

		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetFName();
		ActorData.Transform = Actor->GetActorTransform();

		FMemoryWriter MemWriter(ActorData.ByteData);
		
		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
		
		Ar.SetIsSaving(true);

		// 직렬화
		Actor->Serialize(Ar);

		CurrentSaveGame->SavedActors.Add(ActorData);
	}

	// 게임 저장
	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, CurrentSlotName, 0);

	OnSaveGameWritten.Broadcast(CurrentSaveGame);
}

void USaveGameInstanceSubsystem::LoadSaveGame(FString InSlotName)
{
	SetSlotName(InSlotName);

	if (UGameplayStatics::DoesSaveGameExist(CurrentSlotName, 0))
	{
		CurrentSaveGame = Cast<UAA_SaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame Data."));
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("Loaded SaveGame Data."));

		for (FActorIterator It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;
			// USaveGameInterface를 상속받은 Actor만 불러오기
			if (!Actor->Implements<USaveGameInterface>())
			{
				continue;
			}

			for (FActorSaveData ActorData : CurrentSaveGame->SavedActors)
			{
				if (ActorData.ActorName == Actor->GetFName())
				{
					UE_LOG(LogTemp, Warning, TEXT("Actor Name: %s"), *Actor->GetName());
					Actor->SetActorTransform(ActorData.Transform);

					FMemoryReader MemReader(ActorData.ByteData);

					FObjectAndNameAsStringProxyArchive Ar(MemReader, true);

					Ar.SetIsLoading(true);

					Actor->Serialize(Ar);
					
					ISaveGameInterface::Execute_OnActorLoaded(Actor);

					break;
				}
			}
		}

		OnSaveGameLoaded.Broadcast(CurrentSaveGame);
	}
	else
	{
		// 현재 SaveGame이 없다면 생성한다.
		CurrentSaveGame = Cast<UAA_SaveGame>(UGameplayStatics::CreateSaveGameObject(UAA_SaveGame::StaticClass()));

		UE_LOG(LogTemp, Log, TEXT("Created New SaveGame Data."));
	}
}

bool USaveGameInstanceSubsystem::OverrideSpawnTransform(AController* NewPlayer)
{
	if (!IsValid(NewPlayer))
	{
		return false;
	}

	APawn* PlayerPawn = NewPlayer->GetPawn();
	if (PlayerPawn)
	{
		if (UGameplayStatics::DoesSaveGameExist(CurrentSlotName, 0))
		{
			CurrentSaveGame = Cast<UAA_SaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSlotName, 0));
			if (CurrentSaveGame == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame Data."));
				return false;
			}

			if (CurrentSaveGame->SavedPlayer.ActorName == FName(""))
			{
				return false;

			}

			PlayerPawn->SetActorTransform(CurrentSaveGame->SavedPlayer.Transform);
			PlayerPawn->GetController()->SetControlRotation(PlayerPawn->GetActorRotation());

			FMemoryReader MemReader(CurrentSaveGame->SavedPlayer.ByteData);

			FObjectAndNameAsStringProxyArchive Ar(MemReader, true);

			Ar.SetIsLoading(true);

			PlayerPawn->Serialize(Ar);

			return true;
		}
	}

	return false;
}

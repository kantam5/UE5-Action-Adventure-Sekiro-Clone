// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstances/SaveGameInstanceSubsystem.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Continue->OnClicked.AddDynamic(this, &UMainMenu::OnContinueButtonClicked);
	Button_NewGame->OnClicked.AddDynamic(this, &UMainMenu::OnNewGameButtonClicked);
	Button_QuitGame->OnClicked.AddDynamic(this, &UMainMenu::OnQuitGameButtonClicked);
}

void UMainMenu::OnContinueButtonClicked()
{
	USaveGameInstanceSubsystem* SG = GetGameInstance()->GetSubsystem<USaveGameInstanceSubsystem>();
	if (!SG)
	{
		return;
	}

	UGameplayStatics::OpenLevel(this, FName("ThirdPersonMap"), false);
	
	FString SaveSlotName = SG->GetCurrentSlotName();
	SG->LoadSaveGame(SaveSlotName);
}

void UMainMenu::OnNewGameButtonClicked()
{
	USaveGameInstanceSubsystem* SG = GetGameInstance()->GetSubsystem<USaveGameInstanceSubsystem>();
	if (!SG)
	{
		return;
	}

	FString SaveGameSlot = SG->GetCurrentSlotName();

	UGameplayStatics::DeleteGameInSlot(SaveGameSlot, 0);
	UGameplayStatics::OpenLevel(this, FName("ThirdPersonMap"), false);
}

void UMainMenu::OnQuitGameButtonClicked()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, false);
}

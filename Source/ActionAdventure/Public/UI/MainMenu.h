// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Continue;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_NewGame;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_QuitGame;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnContinueButtonClicked();
	UFUNCTION()
	void OnNewGameButtonClicked();
	UFUNCTION()
	void OnQuitGameButtonClicked();
};

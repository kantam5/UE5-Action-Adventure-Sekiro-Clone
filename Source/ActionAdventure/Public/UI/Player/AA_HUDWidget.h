// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "AA_HUDWidget.generated.h"

class UCanvasPanel;
class UProgressBar;
class UImage;
class UTextBlock;
class ABossCharacter;
class UHudItemSlot;

UCLASS()
class ACTIONADVENTURE_API UAA_HUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Player
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* CanvasPanel_PlayerHealth;
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* CanvasPanel_PlayerStamina;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar_PlayerHealth;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar_PlayerStamina;

	float PlayerMaxHealth;
	float PlayerMaxStamina;

	// Boss
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* CanvasPanel_BossHealth;
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* CanvasPanel_BossStamina;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar_BossHealth;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar_BossStamina;
	UPROPERTY(meta = (BindWidget))
	UImage* BossDeathCount_First;
	UPROPERTY(meta = (BindWidget))
	UImage* BossDeathCount_Second;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_BossName;

	int32 BossMaxDeathCountNum;
	int32 BossDeathCountNum;

	float BossMaxHealth;
	float BossMaxStamina;

	UPROPERTY(meta = (BindWidget))
	UHudItemSlot* WBP_HudItemSlot_Skill;
	UPROPERTY(meta = (BindWidget))
	UHudItemSlot* WBP_HudItemSlot_Potion;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_Mana;

	virtual void NativeConstruct() override;

	void SetBossUI(ABossCharacter* Boss);

	void SetPlayerHealthPercentage(float HealthPercentage);
	void SetPlayerStaminaPercentage(float StaminaPercentage);
	FORCEINLINE void SetPlayerMaxHealth(float InMaxHealth) { PlayerMaxHealth = InMaxHealth; }
	FORCEINLINE void SetPlayerMaxStamina(float InMaxStamina) { PlayerMaxStamina = InMaxStamina; }

	void SetBossUIVisibility(bool InState);

	void SetBossHealthPercentage(float HealthPercentage);
	void SetBossStaminaPercentage(float StaminaPercentage);
	void DeductBossDeathCount();
	void AddBossDeathCount();
	void InitBossDeathCountNumber(float InDeathCount);
	void SetBossNameTextBlock(FText InBossName);
	FORCEINLINE void SetBossMaxHealth(float InMaxHealth) { BossMaxHealth = InMaxHealth; }
	FORCEINLINE void SetBossMaxStamina(float InMaxStamina) { BossMaxStamina = InMaxStamina; }

	void OnPlayerHealthChanged(const FOnAttributeChangeData& Data);
	void OnPlayerStaminaChanged(const FOnAttributeChangeData& Data);
	void OnPlayerManaChanged(const FOnAttributeChangeData& Data);
	
	void OnBossHealthChanged(const FOnAttributeChangeData& Data);
	void OnBossStaminaChanged(const FOnAttributeChangeData& Data);
	void OnBossDeathCountChanged(const FOnAttributeChangeData& Data);
};

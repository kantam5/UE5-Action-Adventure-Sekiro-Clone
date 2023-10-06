// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/AA_HUDWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Characters/PlayerCharacter.h"
#include "Characters/BossCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/AA_AttributeSetBase.h"

void UAA_HUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());
	UAA_AttributeSetBase* PlayerAttributeSet = PlayerCharacter->GetAttributeSet();
	PlayerCharacter->GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(PlayerAttributeSet->GetHealthAttribute()).AddUObject(this, &UAA_HUDWidget::OnPlayerHealthChanged);
	PlayerCharacter->GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(PlayerAttributeSet->GetStaminaAttribute()).AddUObject(this, &UAA_HUDWidget::OnPlayerStaminaChanged);
	PlayerCharacter->GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(PlayerAttributeSet->GetManaAttribute()).AddUObject(this, &UAA_HUDWidget::OnPlayerManaChanged);

}

void UAA_HUDWidget::SetBossUI(ABossCharacter* Boss)
{
	UAA_AttributeSetBase* BossAttributeSet = Boss->GetAttributeSet();
	Boss->GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(BossAttributeSet->GetHealthAttribute()).AddUObject(this, &UAA_HUDWidget::OnBossHealthChanged);
	Boss->GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(BossAttributeSet->GetStaminaAttribute()).AddUObject(this, &UAA_HUDWidget::OnBossStaminaChanged);
	Boss->GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(BossAttributeSet->GetDeathCountAttribute()).AddUObject(this, &UAA_HUDWidget::OnBossDeathCountChanged);

}

void UAA_HUDWidget::SetPlayerHealthPercentage(float HealthPercentage)
{
	ProgressBar_PlayerHealth->SetPercent(HealthPercentage);
}

void UAA_HUDWidget::SetPlayerStaminaPercentage(float StaminaPercentage)
{
	ProgressBar_PlayerStamina->SetPercent(1.0 - StaminaPercentage);
}

void UAA_HUDWidget::SetBossUIVisibility(bool InState)
{
	if (InState)
	{
		CanvasPanel_BossHealth->SetVisibility(ESlateVisibility::Visible);
		CanvasPanel_BossStamina->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CanvasPanel_BossHealth->SetVisibility(ESlateVisibility::Hidden);
		CanvasPanel_BossStamina->SetVisibility(ESlateVisibility::Hidden);
	}

}

void UAA_HUDWidget::SetBossHealthPercentage(float HealthPercentage)
{
	ProgressBar_BossHealth->SetPercent(HealthPercentage);
}


void UAA_HUDWidget::SetBossStaminaPercentage(float StaminaPercentage)
{
	ProgressBar_BossStamina->SetPercent(1.0 - StaminaPercentage);
}

void UAA_HUDWidget::DeductBossDeathCount()
{
	if (BossDeathCountNum == 2)
	{
		BossDeathCount_Second->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f));
		BossDeathCountNum--;
	}
	else
	{
		BossDeathCount_First->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f));
		BossDeathCountNum--;
	}
}

void UAA_HUDWidget::AddBossDeathCount()
{
	BossDeathCountNum++;
	if (BossDeathCountNum == 1)
	{
		BossDeathCount_First->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f));
	}
	else if (BossDeathCountNum == 2)
	{
		BossDeathCount_Second->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f));
	}
}

void UAA_HUDWidget::InitBossDeathCountNumber(float InDeathCount)
{
	if (FMath::IsNearlyEqual(InDeathCount, 2.f))
	{
		BossMaxDeathCountNum = 2;
		BossDeathCountNum = 2;
		BossDeathCount_First->SetVisibility(ESlateVisibility::Visible);
		BossDeathCount_Second->SetVisibility(ESlateVisibility::Visible);
	}
	else if (FMath::IsNearlyEqual(InDeathCount, 1.f))
	{
		BossMaxDeathCountNum = 1;
		BossDeathCountNum = 1;
		BossDeathCount_First->SetVisibility(ESlateVisibility::Visible);
		BossDeathCount_Second->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UAA_HUDWidget::SetBossNameTextBlock(FText InBossName)
{
	TextBlock_BossName->SetText(InBossName);
}

void UAA_HUDWidget::OnPlayerHealthChanged(const FOnAttributeChangeData& Data)
{
	SetPlayerHealthPercentage(Data.NewValue / PlayerMaxHealth);
}

void UAA_HUDWidget::OnPlayerStaminaChanged(const FOnAttributeChangeData& Data)
{
	SetPlayerStaminaPercentage(Data.NewValue / PlayerMaxStamina);
}

void UAA_HUDWidget::OnPlayerManaChanged(const FOnAttributeChangeData& Data)
{
	int32 CurrentMana = Data.NewValue;
	TextBlock_Mana->SetText(FText(FText::FromString(FString::Printf(TEXT("%d"), CurrentMana))));
}

void UAA_HUDWidget::OnBossHealthChanged(const FOnAttributeChangeData& Data)
{
	SetBossHealthPercentage(Data.NewValue / BossMaxHealth);
}

void UAA_HUDWidget::OnBossStaminaChanged(const FOnAttributeChangeData& Data)
{
	SetBossStaminaPercentage(Data.NewValue / BossMaxStamina);
}

void UAA_HUDWidget::OnBossDeathCountChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue < Data.OldValue)
	{
		DeductBossDeathCount();
	}
	else if (Data.NewValue > Data.OldValue)
	{
		AddBossDeathCount();
	}
}

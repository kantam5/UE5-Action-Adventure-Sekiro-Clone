// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/ItemDurationBox.h"

#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Blueprint/WidgetTree.h"
#include "Characters/PlayerCharacter.h"
#include "AbilitySystemComponent.h"

void UItemDurationBox::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());

	PlayerCharacter->GetAbilitySystemComponent()->RegisterGameplayTagEvent(
		FGameplayTag::RequestGameplayTag(FName("Potion.Damage"), EGameplayTagEventType::NewOrRemoved))
		.AddUObject(this, &UItemDurationBox::OnPotionTagChanged);
	PlayerCharacter->GetAbilitySystemComponent()->RegisterGameplayTagEvent(
		FGameplayTag::RequestGameplayTag(FName("Potion.Health"), EGameplayTagEventType::NewOrRemoved))
		.AddUObject(this, &UItemDurationBox::OnPotionTagChanged);
	PlayerCharacter->GetAbilitySystemComponent()->RegisterGameplayTagEvent(
		FGameplayTag::RequestGameplayTag(FName("Potion.HealthDefense"), EGameplayTagEventType::NewOrRemoved))
		.AddUObject(this, &UItemDurationBox::OnPotionTagChanged);
	PlayerCharacter->GetAbilitySystemComponent()->RegisterGameplayTagEvent(
		FGameplayTag::RequestGameplayTag(FName("Potion.StaminaDefense"), EGameplayTagEventType::NewOrRemoved))
		.AddUObject(this, &UItemDurationBox::OnPotionTagChanged);
}

void UItemDurationBox::OnPotionTagChanged(FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount == 1)
	{
		UImage* ItemDurationImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
		if (ItemDurationImage)
		{
			ItemDurationImage->SetBrush(*ItemIconMap.Find(CallbackTag));
			ItemImageMap.Add(CallbackTag, ItemDurationImage);

			HorizontalBox_ItemDuration->AddChild(ItemDurationImage);
		}
	}
	else if(NewCount == 0)
	{
		UImage* ItemDurationImage = *ItemImageMap.Find(CallbackTag);
		if (ItemDurationImage)
		{
			HorizontalBox_ItemDuration->RemoveChild(ItemDurationImage);
		}
	}
}

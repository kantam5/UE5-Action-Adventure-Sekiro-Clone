// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/ItemObtainedWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "DataAssets/AA_Item.h"
#include "Components/Button.h"
#include "Player/AA_PlayerController.h"
#include "Animation/WidgetAnimation.h"

void UItemObtainedWidget::ShowObtainedItemUI(UAA_Item* InItemType)
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SetRenderOpacity(1.f);

	ItemType = InItemType;

	TextBlock_ItemName->SetText(ItemType->ItemName);
	TextBlock_ItemDescription->SetText(ItemType->ItemDescription);
	Image_ItemIcon->SetBrush(ItemType->ItemIcon);
	
	Button_OK->OnClicked.AddUniqueDynamic(this, &UItemObtainedWidget::OnButtonClicked);

	FadeInAnim->GetEndTime();

	PlayAnimation(FadeInAnim);

}

void UItemObtainedWidget::OnButtonClicked()
{
	PlayAnimation(FadeOutAnim);

	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		PlayerController->EndItemPickupUI();
	}

	GetWorld()->GetTimerManager().SetTimer(FadeOutHandle, FTimerDelegate::CreateLambda([this]()
		{
			RemoveFromParent();
		}), FadeOutAnim->GetEndTime(), false);

	Button_OK->OnClicked.Clear();
}

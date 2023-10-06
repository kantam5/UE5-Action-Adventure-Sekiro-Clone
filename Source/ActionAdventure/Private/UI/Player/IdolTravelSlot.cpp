// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/IdolTravelSlot.h"

#include "Components/TextBlock.h"
#include "Actors/CheckPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/PlayerCharacter.h"
#include "Player/AA_PlayerController.h"

void UIdolTravelSlot::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	ACheckPoint* InIdol = Cast<ACheckPoint>(ListItemObject);
	if (InIdol)
	{
		SetSlotInfo(InIdol);
	}
}

FReply UIdolTravelSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (Idol)
	{
		Idol->Rest();
		Idol->Travel();
	}

	return Reply;
}

void UIdolTravelSlot::SetSlotInfo(ACheckPoint* InIdol)
{
	Idol = InIdol;
	TextBlock_IdolName->SetText(InIdol->IdolName);
}

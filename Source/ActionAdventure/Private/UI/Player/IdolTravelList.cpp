// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/IdolTravelList.h"

#include "Components/ListView.h"
#include "GameInstances/AA_GameInstance.h"
#include "Actors/CheckPoint.h"

void UIdolTravelList::NativeConstruct()
{
	Super::NativeConstruct();

	AddTravelSlots();

	UWorld* World = GetWorld();
	UAA_GameInstance* GameInstance = World ? World->GetGameInstance<UAA_GameInstance>() : nullptr;
	if (GameInstance)
	{
		GameInstance->OnCheckPointAdded.AddUniqueDynamic(this, &UIdolTravelList::OnCheckPointAdded);
	}
}

void UIdolTravelList::AddTravelSlots()
{
	ListView_IdolTravelList->ClearListItems();

	UWorld* World = GetWorld();
	UAA_GameInstance* GameInstance = World ? World->GetGameInstance<UAA_GameInstance>() : nullptr;
	if (GameInstance)
	{
		for (ACheckPoint* Idol : GameInstance->CheckPoints)
		{
			ListView_IdolTravelList->AddItem(Idol);
		}
	}
}

void UIdolTravelList::OnCheckPointAdded(ACheckPoint* InCheckPoint)
{
	UWorld* World = GetWorld();
	UAA_GameInstance* GameInstance = World ? World->GetGameInstance<UAA_GameInstance>() : nullptr;
	if (!GameInstance)
	{
		return;
	}

	if (GameInstance->CheckPoints.Find(InCheckPoint))
	{
		ListView_IdolTravelList->AddItem(InCheckPoint);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IdolTravelList.generated.h"

class UListView;
class ACheckPoint;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UIdolTravelList : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UListView* ListView_IdolTravelList;

	virtual void NativeConstruct() override;

	void AddTravelSlots();

	UFUNCTION()
	void OnCheckPointAdded(ACheckPoint* InCheckPoint);
};

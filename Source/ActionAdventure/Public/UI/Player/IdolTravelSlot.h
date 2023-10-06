// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "IdolTravelSlot.generated.h"

class UTextBlock;
class ACheckPoint;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UIdolTravelSlot : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_IdolName;

	UPROPERTY()
	ACheckPoint* Idol;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void SetSlotInfo(ACheckPoint* InIdol);
};

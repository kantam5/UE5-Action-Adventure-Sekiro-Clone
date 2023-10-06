// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONADVENTURE_API IInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void OnScreenBounds() = 0;
	virtual void OnOutScreenBounds() = 0;
	virtual void StartFocus() = 0;
	virtual void EndFocus() = 0;
	virtual bool OnInteract(AActor* Caller) = 0;
};

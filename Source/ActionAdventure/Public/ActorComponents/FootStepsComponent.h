// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AA_GameTypes.h"
#include "FootStepsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONADVENTURE_API UFootStepsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFootStepsComponent();

	UPROPERTY(EditDefaultsOnly)
	FName LeftFootSocketName;
	UPROPERTY(EditDefaultsOnly)
	FName RightFootSocketName;

	UPROPERTY(EditAnywhere)
	float FootStepVolumeMultiplier;

	void HandleFootStep(EFoot Foot);
		
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AA_CharacterTypes.h"
#include "HitSoundParticleDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UHitSoundParticleDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	FHitSoundParticleData HitSoundParticleData;
};

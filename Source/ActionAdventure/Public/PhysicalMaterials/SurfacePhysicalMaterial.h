// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "SurfacePhysicalMaterial.generated.h"

class USoundBase;
class UNiagaraSystem;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API USurfacePhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PhysicalMaterial)
	USoundBase* FootStepSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PhysicalMaterial)
	UNiagaraSystem* FootStepVFX;
};

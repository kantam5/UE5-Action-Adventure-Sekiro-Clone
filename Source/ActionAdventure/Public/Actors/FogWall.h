// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FogWall.generated.h"

UCLASS()
class ACTIONADVENTURE_API AFogWall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFogWall();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* FogWallMeshComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};

// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/FogWall.h"

// Sets default values
AFogWall::AFogWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	FogWallMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FogWallMeshComponent"));
	FogWallMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FogWallMeshComponent->SetHiddenInGame(true);
	RootComponent = FogWallMeshComponent;
}

// Called when the game starts or when spawned
void AFogWall::BeginPlay()
{
	Super::BeginPlay();
	
}


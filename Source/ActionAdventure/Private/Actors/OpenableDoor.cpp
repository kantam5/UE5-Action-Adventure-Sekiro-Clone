// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/OpenableDoor.h"

#include "Kismet/GameplayStatics.h"
#include "DataAssets/AA_KeyItem.h"
#include "Player/AA_PlayerController.h"
#include "Components/WidgetComponent.h"

AOpenableDoor::AOpenableDoor()
	: bNeedKey(false)
	, bOpend(false)
{
	PrimaryActorTick.bCanEverTick = true;

	RightDoorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoorMeshComponent"));
	RightDoorMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RightDoorMeshComponent->SetupAttachment(RootComponent);

	LeftDoorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoorMeshComponent"));
	LeftDoorMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	LeftDoorMeshComponent->SetupAttachment(RootComponent);

}

bool AOpenableDoor::OnInteract(AActor* Caller)
{
	if (bNeedKey)
	{
		AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (PlayerController)
		{
			if (PlayerController->GetInventoryItemCount(KeyItemType) > 0)
			{
				OpenDoor();

				return true;
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		OpenDoor();

		return true;
	}

	return false;
}

void AOpenableDoor::BeginPlay()
{
	Super::BeginPlay();

	if (OpenDoorCurve != nullptr)
	{
		FOnTimelineFloat OpenDoorCurveCallback;

		OpenDoorCurveCallback.BindUFunction(this, FName("OpenDoorTimelineProgress"));

		OpenDoorTimeline.AddInterpFloat(OpenDoorCurve, OpenDoorCurveCallback);

		OpenDoorTimeline.SetTimelineLength(0.5f);
	}
}

void AOpenableDoor::OpenDoor()
{
	HideWidget();
	bOpend = true;
	bCanInteract = false;
	OpenDoorTimeline.PlayFromStart();
}

void AOpenableDoor::OpenDoorTimelineProgress(float Value)
{
	LeftDoorMeshComponent->SetRelativeRotation(FRotator(0.f, (-1.f) * Value, 0.f));
	RightDoorMeshComponent->SetRelativeRotation(FRotator(0.f, Value, 0.f));
}

void AOpenableDoor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (OpenDoorTimeline.IsPlaying())
	{
		OpenDoorTimeline.TickTimeline(DeltaSeconds);
	}
}

void AOpenableDoor::OnActorLoaded_Implementation()
{
	if (bOpend)
	{
		OpenDoor();
	}
}

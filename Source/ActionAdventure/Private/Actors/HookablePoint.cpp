// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/HookablePoint.h"

#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "UI/Hookable/HookablePointWidget.h"

constexpr bool g_DrawDebugHelpers = false;

AHookablePoint::AHookablePoint()
	: LineTraceLength(200.f)
	, DepthLocOffset(0.f)
	, MaxUpdateDistance(0.f)
	, MinUpdateDistance(0.f)
	, LandPointLoc(FVector::ZeroVector)
	, bIsValid(false)
	, bIsHookable(false)
{
	PrimaryActorTick.bCanEverTick = true;

	RootPoint = CreateDefaultSubobject<UBillboardComponent>(TEXT("Root Point"));
	HookPoint = CreateDefaultSubobject<UBillboardComponent>(TEXT("Hook Point"));
	LandPoint = CreateDefaultSubobject<UBillboardComponent>(TEXT("Land Point"));
	UITracker = CreateDefaultSubobject<UHookablePointWidget>(TEXT("UI Tracker"));

	ActivableArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Activable Area"));

	VisibilityCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Visibilit Collision"));

	DirectionalArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Directional Arrow"));

	RootComponent = RootPoint;
	HookPoint->SetupAttachment(RootComponent);
	LandPoint->SetupAttachment(RootComponent);
	UITracker->SetupAttachment(RootComponent);
	ActivableArea->SetupAttachment(RootComponent);
	VisibilityCollision->SetupAttachment(RootComponent);
	DirectionalArrow->SetupAttachment(RootComponent);

	VisibilityCollision->InitSphereRadius(60.f);
}

void AHookablePoint::BeginPlay()
{
	Super::BeginPlay();
	
	RootPoint->SetHiddenInGame(!g_DrawDebugHelpers);
	HookPoint->SetHiddenInGame(!g_DrawDebugHelpers);
	LandPoint->SetHiddenInGame(!g_DrawDebugHelpers);
	VisibilityCollision->SetHiddenInGame(!g_DrawDebugHelpers);

	UITracker->CustomInit();
	CalculateHookAndLandLoc();
}

void AHookablePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!GetWorld()->IsPlayInEditor())
	{
		CalculateHookAndLandLoc();
	}
}

bool AHookablePoint::ShouldTickIfViewportsOnly() const
{
	return false;
}

void AHookablePoint::CustomUpdate(float CharacterPointDistance)
{
	UITracker->CustomUpdate(UITracker->GetComponentLocation(), CharacterPointDistance);

	bIsHookable = UITracker->CheckIfIsHookable();
}

void AHookablePoint::SetCharacterFocused(bool InState)
{
	UITracker->SetCharacterFocused(InState);
}

void AHookablePoint::CalculateHookAndLandLoc()
{
	bool DrawDebugInPlay;

	if (GetWorld()->IsPlayInEditor())
	{
		DrawDebugInPlay = g_DrawDebugHelpers;
	}
	else
	{
		DrawDebugInPlay = true;
	}

	FVector TraceEndOffset = FVector(0.0f, 0.0f, -1.0f) * LineTraceLength;

	FCollisionQueryParams TraceParams(TEXT("HookablePointTrace"), false, this);
	TraceParams.bReturnPhysicalMaterial = false;

	FVector HookTraceStart = HookPoint->GetComponentLocation();
	FVector HookTraceEnd = HookTraceStart + TraceEndOffset;
	FHitResult HookTraceHitDetails = FHitResult(ForceInit);

	bool HookTraceHited = GetWorld()->LineTraceSingleByChannel(
		HookTraceHitDetails,
		HookTraceStart,
		HookTraceEnd,
		ECC_GameTraceChannel3,
		TraceParams);
	
	if (HookTraceHited)
	{
		// NOTE: Hook Point Trace Line
		DrawDebugLine(GetWorld(), HookTraceStart, HookTraceHitDetails.Location,
			FColor::Red, DrawDebugInPlay, -1.0f, 0, 2.0f);

		// NOTE: Hook Point Loc
		DrawDebugPoint(GetWorld(), HookTraceHitDetails.Location, 20.0f,
			FColor::Green, DrawDebugInPlay, -1.0f);

		UITracker->SetWorldLocation(HookTraceHitDetails.Location);
	}
	else
	{
		// NOTE: Hook Point Trace Line
		DrawDebugLine(GetWorld(), HookTraceStart, HookTraceEnd, FColor::Red,
			DrawDebugInPlay, -1.0f, 0, 2.0f);
	}

	FVector LandTraceStart = LandPoint->GetComponentLocation();
	FVector LandTraceEnd = LandTraceStart + TraceEndOffset;
	FHitResult LandTraceHitDetails = FHitResult(ForceInit);

	bool LandTraceHited = GetWorld()->LineTraceSingleByChannel(
		LandTraceHitDetails,
		LandTraceStart,
		LandTraceEnd,
		ECC_GameTraceChannel3,
		TraceParams);
	if (LandTraceHited)
	{
		// NOTE: Land Point Trace Line
		DrawDebugLine(GetWorld(), LandTraceStart, LandTraceHitDetails.Location,
			FColor::Red, DrawDebugInPlay, -1.0f, 0, 2.0f);

		// NOTE: Land Loc
		DrawDebugPoint(GetWorld(),
			LandTraceHitDetails.Location + FVector(0.0f, 0.0f, 97.0f),
			20.0f, FColor::Blue, DrawDebugInPlay, -1.0f);

		LandPointLoc = LandTraceHitDetails.Location + FVector(0.0f, 0.0f, 90.0f);
	}
	else
	{
		// NOTE: Land Point Trace Line
		DrawDebugLine(GetWorld(), LandTraceStart, LandTraceEnd, FColor::Red,
			DrawDebugInPlay, -1.0f, 0, 2.0f);
	}

	if (LandTraceHited && HookTraceHited)
	{
		bIsValid = true;
	}
	else
	{
		bIsValid = false;
	}
}

void AHookablePoint::HidePoint()
{
	UITracker->HideTrackers();
}

void AHookablePoint::PlayInputAnimation()
{
	UITracker->PlayInputAnimation();
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CheckPoint.h"

#include "NiagaraComponent.h"
#include "GameInstances/AA_GameInstance.h"
#include "Player/AA_PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BillboardComponent.h"
#include "Components/ArrowComponent.h"
#include "Characters/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameInstances/SaveGameInstanceSubsystem.h"

ACheckPoint::ACheckPoint()
	: IdolName()
	, LineTraceLength(300.f)
	, SpawnTransform(FTransform())
{
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);

	SpawnPoint = CreateDefaultSubobject<UBillboardComponent>(TEXT("Spawn Point"));
	SpawnPoint->SetupAttachment(RootComponent);

	DirectionalArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Directional Arrow"));
	DirectionalArrow->SetupAttachment(SpawnPoint);
}

bool ACheckPoint::OnInteract(AActor* Caller)
{
	if (!Super::OnInteract(Caller))
	{
		return false;
	}

	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	UWorld* World = GetWorld();
	UAA_GameInstance* GameInstance = World ? World->GetGameInstance<UAA_GameInstance>() : nullptr;
	if (!GameInstance)
	{
		return false;
	}

	if (GameInstance->CheckPoints.Find(this) == INDEX_NONE)
	{
		GameInstance->CheckPoints.Add(this);

		if (PlayerController)
		{
			PlayerController->OnIdolFound();
		}

		GameInstance->OnCheckPointAdded.Broadcast(this);
	}
	else
	{
		if (PlayerController)
		{
			PlayerController->ShowIdolUI(this);
		}
	}

	GameInstance->LatestCheckPoint = this;

	return true;
}

void ACheckPoint::Rest()
{
	UWorld* World = GetWorld();
	UAA_GameInstance* GameInstance = World ? World->GetGameInstance<UAA_GameInstance>() : nullptr;
	if (GameInstance)
	{
		GameInstance->OnRestInCheckPoint.Broadcast();
	}

	USaveGameInstanceSubsystem* SG = GetGameInstance()->GetSubsystem<USaveGameInstanceSubsystem>();
	SG->WriteSaveGame();
}

void ACheckPoint::Travel()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerCharacter)
	{
		PlayerCharacter->SetActorRotation(SpawnTransform.GetRotation());
		PlayerCharacter->SetActorLocation(SpawnTransform.GetLocation());

		PlayerCharacter->GetCameraBoom()->SetRelativeRotation(SpawnTransform.GetRotation());
	}

	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController)
	{
		PlayerController->EndIdolUI();
	}
}

void ACheckPoint::BeginPlay()
{
	Super::BeginPlay();

	CalculateSpawnTransform();
}

void ACheckPoint::CalculateSpawnTransform()
{
	FVector TraceEndOffset = FVector(0.0f, 0.0f, -1.0f) * LineTraceLength;

	FCollisionQueryParams TraceParams(TEXT("SpawnPointTrace"), false, this);

	FVector SpawnTraceStart = SpawnPoint->GetComponentLocation();
	FVector SpawnTraceEnd = SpawnTraceStart + TraceEndOffset;
	FHitResult SpawnTraceHitDetails = FHitResult(ForceInit);

	bool SpawnTraceHited = GetWorld()->LineTraceSingleByChannel(
		SpawnTraceHitDetails,
		SpawnTraceStart,
		SpawnTraceEnd,
		ECC_GameTraceChannel3,
		TraceParams);
	if (SpawnTraceHited)
	{
		SpawnTransform.SetLocation(SpawnTraceHitDetails.Location + FVector(0.f, 0.f, 90.f));
		SpawnTransform.SetRotation(SpawnPoint->GetRelativeRotation().Quaternion());
	}
}

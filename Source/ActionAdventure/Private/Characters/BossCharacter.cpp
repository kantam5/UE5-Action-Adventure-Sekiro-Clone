// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BossCharacter.h"

#include "AbilitySystem/AttributeSets/AA_AttributeSetBase.h"
#include "AbilitySystem/Components/AA_AbilitySystemComponent.h"
#include "AnimInstances/AA_AnimInstance.h"
#include "Components/BoxComponent.h"
#include "AI/AA_AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/PlayerCharacter.h"
#include "AnimInstances/AA_AnimInstance.h"
#include "UI/Enemy/FloatingStatusBarWidget.h"
#include "Components/WidgetComponent.h"
#include "Player/AA_PlayerController.h"
#include "Actors/FogWall.h"

ABossCharacter::ABossCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, FogWalls()
	, BossName()
{
	DetectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Detection Box"));
	DetectionBox->SetupAttachment(RootComponent);

	bIsArmed = true;
	
}

void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	DetectionBox->OnComponentBeginOverlap.AddDynamic(this, &ABossCharacter::OnDetectionBoxBeginOverlap);
	DetectionBox->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void ABossCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void ABossCharacter::OnDetectionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherComp)
	{
		if (OtherActor == UGameplayStatics::GetPlayerPawn(GetWorld(), 0) && !bHasTarget)
		{
			AAA_AIController* AIController = Cast<AAA_AIController>(GetController());
			if (AIController)
			{
				AIController->SeenPlayer();
			}

			AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
			if (PlayerController->IsValidLowLevel())
			{
				PlayerController->SetBossHUD(this);
			}

			FloatingStatusBar->SetVisibility(ESlateVisibility::Hidden);

			for (AFogWall* FogWall : FogWalls)
			{
				FogWall->FogWallMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				FogWall->FogWallMeshComponent->SetHiddenInGame(false);
			}
			
		}
	}
}

void ABossCharacter::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	Super::OnHealthAttributeChanged(Data);
	
	if (FloatingStatusBar)
	{
		FloatingStatusBar->SetHealthPercentage(Data.NewValue / AttributeSet->GetMaxHealth());
	}
}

void ABossCharacter::OnHealthDepleted()
{
	AttributeSet->SetStamina(0.f);
}

void ABossCharacter::OnStaminaAttributeChanged(const FOnAttributeChangeData& Data)
{
	Super::OnStaminaAttributeChanged(Data);

	if (FloatingStatusBar)
	{
		FloatingStatusBar->SetStaminaPercentage(Data.NewValue / AttributeSet->GetMaxStamina());
	}
}

void ABossCharacter::OnStaminaDepleted()
{
	Super::OnStaminaDepleted();

	if (AnimInstance->IsValidLowLevel())
	{
		AnimInstance->PlayGroggyMontage();
	}

	
}

void ABossCharacter::OnDeathCountAttributeChanged(const FOnAttributeChangeData& Data)
{
	Super::OnDeathCountAttributeChanged(Data);

	if (Data.NewValue < Data.OldValue)
	{
		if (FloatingStatusBar)
		{
			FloatingStatusBar->DeductDeathCount();
		}

		AAA_AIController* AIController = Cast<AAA_AIController>(GetController());
		if (AIController)
		{
			AIController->SetSecondPhase();
		}
	}
	else if(Data.NewValue > Data.OldValue)
	{
		if (FloatingStatusBar)
		{
			FloatingStatusBar->AddDeathCount();
		}
	}
}

void ABossCharacter::OnDeathCountDepleted()
{
	PlayFinishedGroggyMontage();

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerCharacter->IsValidLowLevel())
	{
		PlayerCharacter->SetFinishDeathBlowState(this);
	}
}

void ABossCharacter::OnGroggyMontageComplete()
{
	Super::OnGroggyMontageComplete();
}

void ABossCharacter::Die()
{
	Super::Die();

	DetectionBox->OnComponentBeginOverlap.Clear();

	bResetOnRest = false;

	for (AFogWall* FogWall : FogWalls)
	{
		FogWall->Destroy();
	}
}

void ABossCharacter::ResetCharacterToOrigin()
{
	Super::ResetCharacterToOrigin();

	for (AFogWall* FogWall : FogWalls)
	{
		FogWall->FogWallMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FogWall->FogWallMeshComponent->SetHiddenInGame(true);
	}
	
}

void ABossCharacter::PlayFinishedGroggyMontage()
{
	if (!AbilitySystemComponent->HasMatchingGameplayTag(GroggyTag))
	{
		AbilitySystemComponent->AddLooseGameplayTag(GroggyTag);
	}

	if (AnimInstance->IsValidLowLevel())
	{
		PlayAnimMontage(FinishedGroggyMontage);
		FOnMontageEnded BlendOutDelegate;
		BlendOutDelegate.BindUObject(this, &ABossCharacter::OnFinishedGroggyMontageBelndOut);
		AnimInstance->Montage_SetBlendingOutDelegate(BlendOutDelegate, FinishedGroggyMontage);
	}

	SetDeathBlowTrackerVisibility(true);

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.2);
}

void ABossCharacter::OnFinishedGroggyMontageBelndOut(UAnimMontage* AnimMontage, bool trigger)
{
	if (!trigger)
	{
		if (AnimMontage == FinishedGroggyMontage)
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(GroggyTag);

			AttributeSet->SetDeathCount(1.f);
			AttributeSet->SetStamina(AttributeSet->GetMaxStamina() / 3.f);

			APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
			if (PlayerCharacter->IsValidLowLevel())
			{
				PlayerCharacter->EndFinisherDeathBlow();
			}

			SetDeathBlowTrackerVisibility(false);

			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
		}
	}
}


void ABossCharacter::PlayFinishedMontage()
{
	if (FinishedMontage->IsValidLowLevel())
	{
		PlayAnimMontage(FinishedMontage);
	}

	AAA_AIController* AIController = Cast<AAA_AIController>(GetController());
	if (AIController)
	{
		AIController->LostPlayer();
	}
	
	SetDeathBlowTrackerVisibility(false);

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
}

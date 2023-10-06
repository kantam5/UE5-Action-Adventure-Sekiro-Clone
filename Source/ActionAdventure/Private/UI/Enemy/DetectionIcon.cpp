// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Enemy/DetectionIcon.h"

#include "AI/AA_AIController.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"

void UDetectionIcon::NativeConstruct()
{
	Super::NativeConstruct();
	
	Player = Cast<AAA_CharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	CanvasPanel->SetVisibility(ESlateVisibility::Hidden);

	bTick = true;
	bSeen = false;

	DetectValuePercent = 0.f;
	DetectionSpeed = 0.f;
}


void UDetectionIcon::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);
	if (bTick)
	{
		if (!bSeen)
		{
			DetectValuePercent = FMath::Clamp(DetectValuePercent - 0.003f, 0.f, 1.f);
			ProgressBar->SetPercent(DetectValuePercent);

			EvaluateSightDetection();
		}
		else if (bSeen)
		{
			DetectValuePercent = FMath::Clamp(FMath::FInterpConstantTo(DetectValuePercent, 1.01f, DeltaTime, DetectionSpeed), 0.f, 1.f);
			ProgressBar->SetPercent(DetectValuePercent);
			DetectionSpeed = 1.f - (FVector::Distance(OwnerCharacter->GetActorLocation(), Player->GetActorLocation()) / OwnerController->AISightRadius);

			EvaluateSightDetection();
		}
	}
}

void UDetectionIcon::CustomInit(AEnemyCharacterBase* InCharacter)
{
	SetOwner(InCharacter);

	if (OwnerCharacter.Get()->IsValidLowLevel())
	{
		OwnerController = Cast<AAA_AIController>(OwnerCharacter->GetController());
		OwnerController->OnPerceptionDetect.AddDynamic(this, &UDetectionIcon::SightRegistered);
		OwnerController->OnPerceptionMiss.AddDynamic(this, &UDetectionIcon::InitDetectWidget);
	}

}

void UDetectionIcon::SightRegistered(bool bHasSeen, float InDetectSpeed)
{
	bSeen = bHasSeen;
	DetectionSpeed = InDetectSpeed;
	CanvasPanel->SetVisibility(ESlateVisibility::Visible);
}

void UDetectionIcon::HideWidget()
{
	CanvasPanel->SetVisibility(ESlateVisibility::Hidden);
}

void UDetectionIcon::InitDetectWidget()
{
	bTick = true;
	bSeen = false;
	SetVisibility(ESlateVisibility::Visible);
	ProgressBar->SetFillColorAndOpacity(FLinearColor(1.f, 1.f, 0.f, 1.f));
}

void UDetectionIcon::SetDetectionIconDetected()
{
	DetectValuePercent = 1.f;
	ProgressBar->SetPercent(DetectValuePercent);
	EvaluateSightDetection();
}

void UDetectionIcon::EvaluateSightDetection()
{
	if (!bSeen && DetectValuePercent <= 0.f)
	{
		HideWidget();
		OwnerController->LostPlayer();
	}
	else
	{
		if (DetectValuePercent >= 1.f)
		{
			ProgressBar->SetFillColorAndOpacity(FLinearColor(1.f, 0.f, 0.f, 1.f));
			bTick = false;
			GetWorld()->GetTimerManager().SetTimer(WidgetVisibilityTimerHandle, FTimerDelegate::CreateLambda([this]()
				{
					SetVisibility(ESlateVisibility::Hidden);
				}), 0.5f, false);

			OwnerController->SeenPlayer();
		}
	}
}

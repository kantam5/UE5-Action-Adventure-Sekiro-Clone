// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/InteractionActorBase.h"

#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Player/StartFocusedWidget.h"

AInteractionActorBase::AInteractionActorBase()
	: ScreenMarginOffset(100.f)
	, ScreenPos(FVector2D())
	, FixedScreenPos(FVector2D())
	, bOutScreenBounds(false)
	, bFocused(false)
	, bCanInteract(true)
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RootComponent = MeshComponent;

	OnScreenBoundsWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("OnScreenBoundsWidgetComponent"));
	OnScreenBoundsWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	OnScreenBoundsWidgetComp->SetDrawSize(FVector2D(40, 40));
	OnScreenBoundsWidgetComp->SetVisibility(false);
	OnScreenBoundsWidgetComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	StartFocusedWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("StartFocusedWidgetComponent"));
	StartFocusedWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	StartFocusedWidgetComp->SetDrawSize(FVector2D(40, 40));
	StartFocusedWidgetComp->SetVisibility(false);
	StartFocusedWidgetComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

}

void AInteractionActorBase::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeFocusedWidget();
}

void AInteractionActorBase::CustomUpdate(AActor* Character)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	bool OnScreen = UGameplayStatics::ProjectWorldToScreen(PlayerController, GetActorLocation(), ScreenPos, false);

	FVector2D PosOffset = FVector2D(OnScreenBoundsWidgetComp->GetWidget()->GetDesiredSize().X * 0.5f,
		OnScreenBoundsWidgetComp->GetWidget()->GetDesiredSize().Y * 0.75f);
	ScreenPos -= PosOffset;
	FixedScreenPos = ScreenPos;

	// À§Á¬ ÄÄÆ÷³ÍÆ®ÀÇ À§Ä¡°¡ ¹üÀ§¿¡¼­ ¹þ¾î³µ´ÂÁö È®ÀÎ
	ConstrainScreenBounds();

	if (!bOutScreenBounds && bCanInteract)
	{
		if (bFocused)
		{
			OnScreenBoundsWidgetComp->SetVisibility(false);
			StartFocusedWidgetComp->SetVisibility(true);
		}
		else
		{
			StartFocusedWidgetComp->SetVisibility(false);
			OnScreenBoundsWidgetComp->SetVisibility(true);
		}
	}
	else
	{
		HideWidget();
	}
}

void AInteractionActorBase::SetCharacterFocused(bool InState)
{
	bFocused = InState;
}

void AInteractionActorBase::ConstrainScreenBounds()
{
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());

	FVector2D ScreenBounds = FVector2D(ViewportSize.X - ScreenMarginOffset * 2.f, ViewportSize.Y/* - ScreenMarginOffset*/);

	bOutScreenBounds = false;

	if (ScreenPos.Y < ScreenMarginOffset)
	{
		ScreenPos.Y = (ScreenMarginOffset - ScreenPos.Y) + ScreenPos.Y;
		bOutScreenBounds = true;
	}
	if (ScreenPos.X < ScreenMarginOffset)
	{
		ScreenPos.X = (ScreenMarginOffset - ScreenPos.X) + ScreenPos.X;
		bOutScreenBounds = true;
	}
	if (ScreenPos.Y > ScreenBounds.Y)
	{
		ScreenPos.Y = ScreenPos.Y - (ScreenPos.Y - ScreenBounds.Y);
		bOutScreenBounds = true;
	}
	if (ScreenPos.X > ScreenBounds.X)
	{
		ScreenPos.X = ScreenPos.X - (ScreenPos.X - ScreenBounds.X);
		bOutScreenBounds = true;
	}
}

void AInteractionActorBase::OnScreenBounds()
{
	OnScreenBoundsWidgetComp->SetVisibility(true);
}

void AInteractionActorBase::OnOutScreenBounds()
{
	OnScreenBoundsWidgetComp->SetVisibility(false);
}

void AInteractionActorBase::StartFocus()
{
	StartFocusedWidgetComp->SetVisibility(true);
}

void AInteractionActorBase::EndFocus()
{
	StartFocusedWidgetComp->SetVisibility(false);
}

bool AInteractionActorBase::OnInteract(AActor* Caller)
{
	if (!CanInteract())
	{
		return false;
	}

	return true;
}

bool AInteractionActorBase::CanInteract() const
{
	return StartFocusedWidgetComp->GetVisibleFlag();
}

void AInteractionActorBase::HideWidget()
{
	if (OnScreenBoundsWidgetComp)
	{
		OnScreenBoundsWidgetComp->SetVisibility(false);
	}
	
	if (StartFocusedWidgetComp)
	{
		StartFocusedWidgetComp->SetVisibility(false);
	}
	
}

void AInteractionActorBase::InitializeFocusedWidget()
{
	if (StartFocusedWidget)
	{
		return;
	}

	if (StartFocusedWidgetClass)
	{
		StartFocusedWidget = CreateWidget<UStartFocusedWidget>(GetWorld(), StartFocusedWidgetClass);
		if (StartFocusedWidget && StartFocusedWidgetComp)
		{
			StartFocusedWidget->SetFocusedText(FocusedText);
			StartFocusedWidgetComp->SetWidget(StartFocusedWidget);
		}
	}

}

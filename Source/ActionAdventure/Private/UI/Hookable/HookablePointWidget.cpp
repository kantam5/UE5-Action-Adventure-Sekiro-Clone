// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Hookable/HookablePointWidget.h"

#include "UI/Hookable/HookablePointTracker.h"
#include "UI/Hookable/HookablePointTrackerMovable.h"
#include "Kismet/GameplayStatics.h"

UHookablePointWidget::UHookablePointWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	Tracker(nullptr),
	MovableTracker(nullptr),
	ScreenPos(FVector::ZeroVector),
	FixedScreenPos(FVector::ZeroVector),
	OutScreenBounds(false)
{
}

// 위젯 초기화
void UHookablePointWidget::CustomInit()
{
	Tracker = Cast<UHookablePointTracker>(GetWidget());
	MovableTracker = CreateWidget<UHookablePointTrackerMovable>(GetWorld(), MovableClass);

	Tracker->InitWidget(StartUpdateDistance, ScaleRateMultiplier);
	MovableTracker->InitWidget(StartUpdateDistance, ScaleRateMultiplier);

	Tracker->GetUnhideDelegate().AddDynamic(this, &UHookablePointWidget::OnTrackerUnhide);
	MovableTracker->GetUnhideDelegate().AddDynamic(this, &UHookablePointWidget::OnTrackerUnhide);
}

// Tick마다 UI를 업데이트
void UHookablePointWidget::CustomUpdate(const FVector& PointWorldPos, float CharacterPointDistance)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	
	bool OnScreen = UGameplayStatics::ProjectWorldToScreen(PlayerController, PointWorldPos, ScreenPos, false);

	FVector2D PosOffset = FVector2D(MovableTracker->GetDesiredSize().X * 0.5f, MovableTracker->GetDesiredSize().Y * 0.75f);
	ScreenPos -= PosOffset;
	FixedScreenPos = ScreenPos;

	// UI위치가 범위에서 벗어났는지 확인
	ConstrainScreenBounds(PointWorldPos);

	// 거리와 위치에따라 UI의 머티리얼을 수정
	Tracker->CustomUpdate(CharacterPointDistance);
	MovableTracker->CustomUpdate(ScreenPos, FixedScreenPos, CharacterPointDistance);

	if (OnScreen)
	{
		if (TrackerActive())
		{
			VisibilityBehavior();
		}
		else
		{
			HideTrackers();
		}
	}
	else
	{
		HideTrackers();
	}
}

void UHookablePointWidget::SetCharacterFocused(bool InState)
{
	if (Tracker)
	{
		Tracker->SetFocused(InState);
	}

	if (MovableTracker)
	{
		MovableTracker->SetFocused(InState);
	}
}

void UHookablePointWidget::HideTrackers()
{
	Tracker->SetRenderOpacity(0.f);
	MovableTracker->SetRenderOpacity(0.f);

	Tracker->ResetWidget();
	MovableTracker->ResetWidget();
}

bool UHookablePointWidget::CheckIfIsHookable() const
{
	if (!Tracker)
	{
		return false;
	}
	if (!MovableTracker)
	{
		return false;
	}

	if ((Tracker->GetIsTrackerFull() && Tracker->GetRenderOpacity() == 1.f) ||
		(MovableTracker->GetIsTrackerFull() && MovableTracker->GetRenderOpacity() == 1.f))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void UHookablePointWidget::PlayInputAnimation()
{
	if (!Tracker)
	{
		return;
	}
	if (!MovableTracker)
	{
		return;
	}

	Tracker->PlayInputAnimation();
	MovableTracker->PlayInputAnimation();
}

void UHookablePointWidget::OnTrackerUnhide(UHookablePointTracker* InTracker)
{
	if (!Tracker)
	{
		return;
	}
}

void UHookablePointWidget::ConstrainScreenBounds(const FVector& PointWorldPos)
{
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());

	FVector2D ScreenBounds = FVector2D(ViewportSize.X - ScreenMarginOffset * 2.f, ViewportSize.Y - ScreenMarginOffset * 2.f);
	
	OutScreenBounds = false;

	if (ScreenPos.Y < ScreenMarginOffset)
	{
		ScreenPos.Y = (ScreenMarginOffset - ScreenPos.Y) + ScreenPos.Y;
		OutScreenBounds = true;
	}
	if (ScreenPos.X < ScreenMarginOffset)
	{
		ScreenPos.X = (ScreenMarginOffset - ScreenPos.X) + ScreenPos.X;
		OutScreenBounds = true;
	}
	if (ScreenPos.Y > ScreenBounds.Y)
	{
		ScreenPos.Y = ScreenPos.Y - (ScreenPos.Y - ScreenBounds.Y);
		OutScreenBounds = true;
	}
	if (ScreenPos.X > ScreenBounds.X)
	{
		ScreenPos.X = ScreenPos.X - (ScreenPos.X - ScreenBounds.X);
		OutScreenBounds = true;
	}
}

bool UHookablePointWidget::TrackerActive() const
{
	if (!Tracker)
	{
		return false;
	}
	if (!MovableTracker)
	{
		return false;
	}

	if (Tracker->GetFocused() && MovableTracker->GetFocused())
	{
		return true;
	}
	else
	{
		return false;
	}

}

void UHookablePointWidget::VisibilityBehavior()
{
	// 화면 가운데에서 벗어났다면 화살표가 있는 트랙커를 보이게 한다.
	if (OutScreenBounds)
	{
		Tracker->SetRenderOpacity(0.f);
		MovableTracker->UnhideTracker();
	}
	else
	{
		MovableTracker->SetRenderOpacity(0.f);
		Tracker->UnhideTracker();
	}
}

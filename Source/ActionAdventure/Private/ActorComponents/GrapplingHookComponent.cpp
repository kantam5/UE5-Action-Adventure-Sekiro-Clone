// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/GrapplingHookComponent.h"

#include "Actors/HookablePoint.h"
#include "Characters/PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

constexpr float g_LastMaxRateOffset = 0.5e-3f;

UGrapplingHookComponent::UGrapplingHookComponent()
	: OwnerCharacter()
	, HookablePoints()
	, FocusedPoint()
	, LastFocusedPoint()
	, LastFocusedPointTrackerLoc()
	, LastFocusedPointLandLoc()
	, HookJumpMontages()
	, CurrentMontage()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGrapplingHookComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	
	TSet<AActor*> OverlappingActors;
	OwnerCharacter->GetOverlappingActors(OverlappingActors, AHookablePoint::StaticClass());
	for (AActor* OverlappingActor : OverlappingActors)
	{
		AHookablePoint* InteractionActor = Cast<AHookablePoint>(OverlappingActor);
		if (InteractionActor)
		{
			HookablePoints.AddUnique(InteractionActor);
		}
	}
}

void UGrapplingHookComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	UpdateHookeablePoints();

}

void UGrapplingHookComponent::UpdateHookeablePoints()
{
	if (HookablePoints.Num() > 0)
	{
		TMap<int, float> PointsFacingRate;

		for (AHookablePoint* HookablePoint : HookablePoints)
		{
			if (HookablePoint == nullptr)
			{
				return;
			}

			// HookablePoint와의 거리와 뷰포트의 일정 영역에 포착되는지 확인
			if (IsCharacterWithinPointBounds(HookablePoint))
			{
				HookablePoint->CustomUpdate(CharacterPointDistance(HookablePoint));

				if (/*IsCharacterInFrontOfThePoint(HookablePoint) && */IsPointVisible(HookablePoint))
				{
					CalcPointFacingRate(HookablePoint, PointsFacingRate);
				}
			}
			else
			{
				HookablePoint->HidePoint();
			}
		}

		if (PointsFacingRate.Num() > 0)
		{
			CalcBestFacingPointRate(PointsFacingRate);
		}
		else
		{
			if (FocusedPoint)
			{
				FocusedPoint->SetCharacterFocused(false);
			}

			FocusedPoint = nullptr;
		}
	}
}

bool UGrapplingHookComponent::IsCharacterWithinPointBounds(const AHookablePoint* HookablePoint) const
{
	if (CharacterPointDistance(HookablePoint) <= HookablePoint->MaxUpdateDistance &&
		CharacterPointDistance(HookablePoint) >= HookablePoint->MinUpdateDistance)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool UGrapplingHookComponent::IsCharacterInFrontOfThePoint(const AHookablePoint* HookablePoint) const
{
	if (HookablePoint->GetActorForwardVector().X * (
		(HookablePoint->GetActorLocation().X + HookablePoint->DepthLocOffset) - OwnerCharacter.Get()->GetActorLocation().X) >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool UGrapplingHookComponent::IsPointVisible(AHookablePoint* HookablePoint) const
{
	FVector CameraForward = UKismetMathLibrary::GetForwardVector(
		OwnerCharacter.Get()->GetCameraBoom()->GetTargetRotation()
		+ OwnerCharacter.Get()->GetFollowCamera()->GetRelativeRotation());

	FCollisionQueryParams TraceParams(TEXT("VisibilityHookablePointTrace"), true);
	TraceParams.bReturnPhysicalMaterial = false;

	FHitResult HitDetails = FHitResult(ForceInit);

	bool Hited = GetWorld()->LineTraceSingleByChannel(
		HitDetails,
		OwnerCharacter.Get()->GetFollowCamera()->GetComponentLocation(),
		HookablePoint->GetTrackerLoc(),
		ECC_Visibility,
		TraceParams);

	if (Hited)
	{
		if (HitDetails.GetActor() == HookablePoint)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

float UGrapplingHookComponent::CharacterPointDistance(const AHookablePoint* HookablePoint) const
{
	FVector CharacterToPoint = HookablePoint->GetTrackerLoc() - OwnerCharacter.Get()->GetActorLocation();

	return CharacterToPoint.Size();
}

void UGrapplingHookComponent::CalcPointFacingRate(AHookablePoint* HookablePoint, TMap<int, float>& PointsFacingRate) const
{
	FVector CameraPointDirection = UKismetMathLibrary::GetDirectionUnitVector(
		OwnerCharacter.Get()->GetFollowCamera()->GetComponentLocation(),
		HookablePoint->GetTrackerLoc());

	FVector CameraForward = UKismetMathLibrary::GetForwardVector(
		OwnerCharacter.Get()->GetCameraBoom()->GetTargetRotation()
		+ OwnerCharacter.Get()->GetFollowCamera()->GetRelativeRotation());

	float PointFacingRate = FVector::DotProduct(CameraForward, CameraPointDirection);
	
	if (PointFacingRate > 0.5f)
	{
		PointsFacingRate.Add(HookablePoints.IndexOfByKey(HookablePoint), PointFacingRate);
	}
}

void UGrapplingHookComponent::CalcBestFacingPointRate(TMap<int, float>& PointsFacingRate)
{
	TPair<int, float> MaxRate(0, 0.f);

	for (TPair<int, float>& it : PointsFacingRate)
	{
		if (it.Value > MaxRate.Value)
		{
			MaxRate = it;
		}
	}

	int NewFocusedPointIndex = MaxRate.Key;

	static TPair<int, float> LastMaxRate(0, 0.f);

	if (FocusedPoint)
	{
		if (HookablePoints[NewFocusedPointIndex] != FocusedPoint)
		{
			if (MaxRate.Key == LastMaxRate.Key ||
				MaxRate.Value >= LastMaxRate.Value + g_LastMaxRateOffset)
			{
				FocusedPoint->SetCharacterFocused(false);
				FocusedPoint = HookablePoints[NewFocusedPointIndex];
				FocusedPoint->SetCharacterFocused(true);
			}
		}
	}
	else
	{
		FocusedPoint = HookablePoints[NewFocusedPointIndex];
		FocusedPoint->SetCharacterFocused(true);
	}

	LastMaxRate = MaxRate;
}

bool UGrapplingHookComponent::CanHookThrow() const
{
	if (FocusedPoint)
	{
		if (FocusedPoint->GetIsHookable())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

UAnimMontage* UGrapplingHookComponent::GetHookMontage()
{
	if (FocusedPoint)
	{
		LastFocusedPoint = FocusedPoint;
		LastFocusedPointTrackerLoc = FocusedPoint->GetTrackerLoc();
		LastFocusedPointLandLoc = FocusedPoint->GetLandLoc();

		FString MontageName = "";
		if (OwnerCharacter.Get()->GetCharacterMovement()->IsFalling())
		{
			MontageName = "Air_";
		}
		else
		{
			MontageName = "Ground_";
		}

		if (LastFocusedPointTrackerLoc.Z - OwnerCharacter.Get()->GetActorLocation().Z <= 0)
		{
			MontageName.Append("LowAltitude");
		}
		else
		{
			MontageName.Append("HighAltitude");
		}

		if (HookJumpMontages)
		{
			static const FString ContextString(TEXT("Hook Jump Montage Context"));
			CurrentMontage = HookJumpMontages->FindRow<FHookMontage>(FName(MontageName), ContextString, true);
			if (CurrentMontage)
			{
				return CurrentMontage->AnimMontage;
			}
		}
	}
	
	return nullptr;
}

void UGrapplingHookComponent::PlayInputAnimation()
{
	if (FocusedPoint)
	{
		FocusedPoint->PlayInputAnimation();
	}
}

void UGrapplingHookComponent::OnCapsuleOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != OwnerCharacter.Get() && OtherComp)
	{
		if (OtherActor->IsA(AHookablePoint::StaticClass()))
		{
			AHookablePoint* HookablePoint = Cast<AHookablePoint>(OtherActor);
			if (HookablePoint->GetIsValid())
			{
				HookablePoints.AddUnique(HookablePoint);
			}
		}
	}
}

void UGrapplingHookComponent::OnCapsuleOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != OwnerCharacter.Get() && OtherComp)
	{
		if (OtherActor->IsA(AHookablePoint::StaticClass()))
		{
			AHookablePoint* HookablePoint = Cast<AHookablePoint>(OtherActor);

			if (HookablePoint == FocusedPoint)
			{
				FocusedPoint = nullptr;
			}

			HookablePoint->HidePoint();
			HookablePoints.RemoveSingleSwap(HookablePoint);
		}
	}
}

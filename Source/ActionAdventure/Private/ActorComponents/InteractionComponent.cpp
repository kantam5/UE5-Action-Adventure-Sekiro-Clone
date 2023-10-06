// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/InteractionComponent.h"

#include "Actors/InteractionActorBase.h"
#include "Characters/PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"

UInteractionComponent::UInteractionComponent()
	: OwnerCharacter()
	, InteractionActors()
	, FocusedActor()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<APlayerCharacter>(GetOwner());

	// 플레이어 스폰 시, 오버랩 체크
	TSet<AActor*> OverlappingActors;
	OwnerCharacter->GetOverlappingActors(OverlappingActors, AInteractionActorBase::StaticClass());
	for (AActor* OverlappingActor : OverlappingActors)
	{
		AInteractionActorBase* InteractionActor = Cast<AInteractionActorBase>(OverlappingActor);
		if (InteractionActor)
		{
			InteractionActors.AddUnique(InteractionActor);
		}
	}
}

bool UInteractionComponent::TryInteract()
{
	if (FocusedActor)
	{
		FocusedActor->OnInteract(OwnerCharacter.Get());
	}

	return false;
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateInteractionActors();
}

void UInteractionComponent::UpdateInteractionActors()
{
	if (InteractionActors.Num() > 0)
	{
		TMap<int, float> ActorsFacingRate;

		for (AInteractionActorBase* InteractionActor : InteractionActors)
		{
			if (InteractionActor == nullptr)
			{
				return;
			}

			// 일단 InteractionActor마다 업데이트 돌린다. (뷰포트에 있는지 거리는 어떻게 되고, 캐릭터의 앞에 있는지)
			InteractionActor->CustomUpdate(OwnerCharacter.Get());

			if (IsActorVisible(InteractionActor))
			{
				CalcPointFacingRate(InteractionActor, ActorsFacingRate);
			}
		}

		if (ActorsFacingRate.Num() > 0)
		{
			CalcBestFacingPointRate(ActorsFacingRate);
		}
		else
		{
			if (FocusedActor)
			{
				FocusedActor->SetCharacterFocused(false);
			}

			FocusedActor = nullptr;
		}
	}
}

bool UInteractionComponent::IsActorVisible(AInteractionActorBase* InteractionActor) const
{
	FVector CameraForward = UKismetMathLibrary::GetForwardVector(
		OwnerCharacter.Get()->GetCameraBoom()->GetTargetRotation()
		+ OwnerCharacter.Get()->GetFollowCamera()->GetRelativeRotation());

	FCollisionQueryParams TraceParams(TEXT("VisibilityInteractionActorTrace"), true);
	TraceParams.bReturnPhysicalMaterial = false;

	FHitResult HitDetails = FHitResult(ForceInit);

	bool Hited = GetWorld()->LineTraceSingleByChannel(
		HitDetails,
		OwnerCharacter.Get()->GetFollowCamera()->GetComponentLocation(),
		InteractionActor->OnScreenBoundsWidgetComp->GetComponentLocation(),
		ECC_Visibility,
		TraceParams);

	if (Hited)
	{
		if (HitDetails.GetActor() == InteractionActor)
		{
			return true;
		}
		else
		{
			InteractionActor->OnOutScreenBounds();
			return false;
		}
	}
	else
	{
		return false;
	}
}

void UInteractionComponent::CalcPointFacingRate(AInteractionActorBase* InteractionActor, TMap<int, float>& ActorsFacingRate) const
{
	if (FVector::Dist(OwnerCharacter.Get()->GetActorLocation(), InteractionActor->GetActorLocation()) <= 200.f)
	{
		FVector OwnerCharacterToActorDirection = UKismetMathLibrary::GetDirectionUnitVector(
			OwnerCharacter.Get()->GetActorLocation(),
			InteractionActor->GetActorLocation());

		FVector OwnerCharacterForward = OwnerCharacter.Get()->GetActorForwardVector();

		float ActorFacingRate = FVector::DotProduct(OwnerCharacterForward, OwnerCharacterToActorDirection);

		if (ActorFacingRate > 0.25f)
		{
			ActorsFacingRate.Add(InteractionActors.IndexOfByKey(InteractionActor), ActorFacingRate);
		}
	}
}

void UInteractionComponent::CalcBestFacingPointRate(TMap<int, float>& ActorsFacingRate)
{
	float LastMaxRateOffset = 0.5e-3f;

	TPair<int, float> MaxRate(0, 0.f);

	for (TPair<int, float>& it : ActorsFacingRate)
	{
		if (it.Value > MaxRate.Value)
		{
			MaxRate = it;
		}
	}

	int NewFocusedActorIndex = MaxRate.Key;

	static TPair<int, float> LastMaxRate(0, 0.f);

	if (FocusedActor)
	{
		if (InteractionActors[NewFocusedActorIndex] != FocusedActor)
		{
			if (MaxRate.Key == LastMaxRate.Key ||
				MaxRate.Value >= LastMaxRate.Value + LastMaxRateOffset)
			{
				FocusedActor->SetCharacterFocused(false);
				FocusedActor = InteractionActors[NewFocusedActorIndex];
				FocusedActor->SetCharacterFocused(true);
			}
		}
	}
	else
	{
		FocusedActor = InteractionActors[NewFocusedActorIndex];
		FocusedActor->SetCharacterFocused(true);
	}

	LastMaxRate = MaxRate;
}


void UInteractionComponent::OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != OwnerCharacter.Get() && OtherComp)
	{
		if (OtherActor->IsA(AInteractionActorBase::StaticClass()))
		{
			AInteractionActorBase* InteractionActor = Cast<AInteractionActorBase>(OtherActor);
			if (InteractionActor && InteractionActor->bCanInteract)
			{
				InteractionActors.AddUnique(InteractionActor);
			}
		}
	}
}

void UInteractionComponent::OnCollisionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != OwnerCharacter.Get() && OtherComp)
	{
		if (OtherActor->IsA(AInteractionActorBase::StaticClass()))
		{
			AInteractionActorBase* InteractionActor = Cast<AInteractionActorBase>(OtherActor);

			if (InteractionActor == FocusedActor)
			{
				FocusedActor = nullptr;
			}
			InteractionActor->HideWidget();
			InteractionActors.RemoveSingleSwap(InteractionActor);
		}
	}
}


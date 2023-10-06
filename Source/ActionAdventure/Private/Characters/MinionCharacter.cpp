// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MinionCharacter.h"

#include "AbilitySystem/Components/AA_AbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/AA_AttributeSetBase.h"
#include "AnimInstances/AA_AnimInstance.h"
#include "Components/SplineComponent.h"
#include "UI/Enemy/FloatingStatusBarWidget.h"
#include "UI/Enemy/DetectionIcon.h"
#include "AI/AA_AIController.h"
#include "Components/WidgetComponent.h"
#include "DataAssets/CharacterAnimDataAsset.h"
#include "DataAssets/AA_Item.h"
#include "Actors/ItemPickupActor.h"

AMinionCharacter::AMinionCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, PatrolPoints()
	, CurrentPatrolPoint(0)
	, bHealthDepleted(false)
	, DropItems()
{
	DetectWidgetComp = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("DetectIcon"));
	DetectWidgetComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	SplineComponent = ObjectInitializer.CreateDefaultSubobject<USplineComponent>(this, TEXT("SplineComponent"));
	SplineComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void AMinionCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeDetectIcon();

	for (int i = 0; i < SplineComponent->GetNumberOfSplinePoints(); i++)
	{
		PatrolPoints.Add(SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World));
	}
	SplineComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void AMinionCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	DetectWidgetComp->SetVisibility(true);
}

void AMinionCharacter::UnPossessed()
{
	Super::UnPossessed();

	DetectWidgetComp->SetVisibility(false);
}

void AMinionCharacter::HandlePoiseDamage(float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags, AAA_CharacterBase* InstigatorCharacter, AActor* DamageCauser)
{
	Super::HandlePoiseDamage(DamageAmount, HitInfo, DamageTags, InstigatorCharacter, DamageCauser);

	if (!bHasTarget)
	{
		if (DetectIcon)
		{
			DetectIcon->SetDetectionIconDetected();
		}
	}
}

void AMinionCharacter::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	Super::OnHealthAttributeChanged(Data);

	if (FloatingStatusBar)
	{
		FloatingStatusBar->SetHealthPercentage(Data.NewValue / AttributeSet->GetMaxHealth());
	}
}

void AMinionCharacter::OnHealthDepleted()
{
	bHealthDepleted = true;

	AttributeSet->SetStamina(0.f);
}

void AMinionCharacter::OnStaminaAttributeChanged(const FOnAttributeChangeData& Data)
{
	Super::OnStaminaAttributeChanged(Data);

	if (FloatingStatusBar)
	{
		FloatingStatusBar->SetStaminaPercentage(Data.NewValue / AttributeSet->GetMaxStamina());
	}

}

void AMinionCharacter::OnStaminaDepleted()
{
	Super::OnStaminaDepleted();

	if (AnimInstance->IsValidLowLevel())
	{
		if (bHealthDepleted)
		{
			AnimInstance->SetIsGroggying(true);
			AnimInstance->Montage_Play(CharacterData.CharacterAnimDataAsset->CharacterAnimationData.DeathMontage);
		}
		else
		{
			AnimInstance->PlayGroggyMontage();
		}
	}
}

void AMinionCharacter::OnDeathCountAttributeChanged(const FOnAttributeChangeData& Data)
{
	Super::OnDeathCountAttributeChanged(Data);

	if (Data.NewValue < Data.OldValue)
	{
		if (FloatingStatusBar)
		{
			FloatingStatusBar->DeductDeathCount();
		}
	}
	else if (Data.NewValue > Data.OldValue)
	{
		if (FloatingStatusBar)
		{
			FloatingStatusBar->AddDeathCount();
		}
	}
}

void AMinionCharacter::OnDeathCountDepleted()
{
	SetDeathBlowTrackerVisibility(false);
	Die();
}

void AMinionCharacter::Die()
{
	Super::Die();

	SpawnItem();
}

void AMinionCharacter::InitializeDetectIcon()
{
	if (DetectIcon)
	{
		return;
	}

	AAA_AIController* AIController = Cast<AAA_AIController>(GetController());
	if (AIController)
	{
		if (DetectIconClass)
		{
			DetectIcon = CreateWidget<UDetectionIcon>(AIController->GetWorld(), DetectIconClass);
			if (DetectIcon && DetectWidgetComp)
			{
				DetectWidgetComp->SetWidget(DetectIcon);
				DetectIcon->CustomInit(this);
			}
		}
	}
}

void AMinionCharacter::SpawnItem()
{
	if (DropItems.Num() > 0)
	{
		int32 DropItemIndex = FMath::RandRange(1, DropItems.Num()) - 1;

		FTransform Transform;
		Transform.SetLocation(GetMesh()->GetComponentLocation());
		Transform.SetRotation(GetActorRotation().Quaternion());
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AItemPickupActor* ItemPickupActor = GetWorld()->SpawnActor<AItemPickupActor>(ItemPickupActorClass, Transform, SpawnParameters);

		ItemPickupActor->SetItemType(DropItems[DropItemIndex]);
		ItemPickupActor->SetDropped(true);
	}
	
}

FVector AMinionCharacter::GetNextMoveLocation()
{
	CurrentPatrolPoint = (CurrentPatrolPoint + 1) % PatrolPoints.Num();

	return PatrolPoints[CurrentPatrolPoint];
}

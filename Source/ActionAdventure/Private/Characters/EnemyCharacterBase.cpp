// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EnemyCharacterBase.h"

#include "Components/WidgetComponent.h"
#include "UI/Enemy/DetectionIcon.h"
#include "UI/Enemy/FloatingStatusBarWidget.h"
#include "AI/AA_AIController.h"
#include "AbilitySystem/Components/AA_AbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/AA_AttributeSetBase.h"
#include "MotionWarpingComponent.h"
#include "AnimInstances/AA_AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameInstances/AA_GameInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyCharacterBase::AEnemyCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bCanDeflect(false)
	, bCounter(false)
	, MaxGuardCount(0)
	, GuardCount(0)
	, bIsLockedOn(false)
	, bIsOverlappingUISphere(false)
	, bHasTarget(false)
	, ResetTransform(FTransform())
	, bResetOnRest(true)
{
	FloatingStatusBarWidgetComp = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("FloatingStatusBar"));
	FloatingStatusBarWidgetComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	FloatingStatusBarWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	FloatingStatusBarWidgetComp->SetDrawSize(FVector2D(500, 500));
	FloatingStatusBarWidgetComp->SetVisibility(false);

	LockOnWidgetComp = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("LockOnTracker"));
	LockOnWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	LockOnWidgetComp->SetDrawSize(FVector2D(18, 18));
	LockOnWidgetComp->SetVisibility(false);

	DeathBlowWidgetComp = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("DeathBlowTracker"));
	DeathBlowWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	DeathBlowWidgetComp->SetDrawSize(FVector2D(40, 40));
	DeathBlowWidgetComp->SetVisibility(false);
	DeathBlowWidgetComp->SetInitialLayerZOrder(-99);

	BackstabDeathBlowWidgetComp = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("BackstabDeathBlowTracker"));
	BackstabDeathBlowWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	BackstabDeathBlowWidgetComp->SetDrawSize(FVector2D(40, 40));
	BackstabDeathBlowWidgetComp->SetVisibility(false);
	BackstabDeathBlowWidgetComp->SetInitialLayerZOrder(-99);

	PlungeDeathBlowWidgetComp = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("PlungeDeathBlowTracker"));
	PlungeDeathBlowWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	PlungeDeathBlowWidgetComp->SetDrawSize(FVector2D(40, 40));
	PlungeDeathBlowWidgetComp->SetVisibility(false);
	PlungeDeathBlowWidgetComp->SetInitialLayerZOrder(-99);

	bIsArmed = false;
	TeamNumber = 1;
}

void AEnemyCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	InitializeFloatingStatusBar();

	if (LockOnWidgetComp->IsValidLowLevel())
	{
		LockOnWidgetComp->SetRelativeLocation(FVector::ZeroVector);
		LockOnWidgetComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("LockOnSocket"));
	}

	if (DeathBlowWidgetComp->IsValidLowLevel())
	{
		DeathBlowWidgetComp->SetRelativeLocation(FVector::ZeroVector);
		DeathBlowWidgetComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("LockOnSocket"));
	}

	if (BackstabDeathBlowWidgetComp->IsValidLowLevel())
	{
		BackstabDeathBlowWidgetComp->SetRelativeLocation(FVector::ZeroVector);
		BackstabDeathBlowWidgetComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("LockOnSocket"));
	}

	if (PlungeDeathBlowWidgetComp->IsValidLowLevel())
	{
		PlungeDeathBlowWidgetComp->SetRelativeLocation(FVector::ZeroVector);
		PlungeDeathBlowWidgetComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("LockOnSocket"));
	}

	ResetTransform.SetLocation(GetActorLocation());
	ResetTransform.SetRotation(GetActorRotation().Quaternion());

	UWorld* World = GetWorld();
	UAA_GameInstance* GameInstance = World ? World->GetGameInstance<UAA_GameInstance>() : nullptr;
	if (GameInstance)
	{
		GameInstance->OnRestInCheckPoint.AddDynamic(this, &AEnemyCharacterBase::OnRestInCheckPoint);
	}
}

void AEnemyCharacterBase::HandleDeathBlow(FGameplayTag Tag, const AAA_CharacterBase* Character)
{
	if (MotionWarpingComponent)
	{
		FRotator EndRot = UKismetMathLibrary::FindLookAtRotation(
			Character->GetActorLocation(), GetActorLocation());
		EndRot.Pitch = 0.f;
		EndRot.Roll = 0.f;

		if (Tag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.DeathBlow.Default")))
		{
			MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
				FName("JumpToInstigator"),
				(Character->GetActorLocation() + Character->GetActorForwardVector() * 200.f),
				EndRot.Add(0.f, 180.f, 0.f));
		}
		else if (Tag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.DeathBlow.Backstab")))
		{
			SetActorLocation(FVector(Character->GetActorLocation().X, Character->GetActorLocation().Y, GetActorLocation().Z)
				+ 80.f * EndRot.Vector());

			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
				FName("JumpToInstigator"),
				(GetActorLocation()),
				EndRot);
		}
		else if (Tag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.DeathBlow.Plunging")))
		{
			MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
				FName("JumpToInstigator"),
				(Character->GetActorLocation() + Character->GetActorForwardVector() * 250.f),
				EndRot);
		}
	}

	if (AnimInstance)
	{
		AnimInstance->PlayDeathBlowReactMontage(Tag);
	}

	SetDeathBlowTrackerVisibility(false);

	AbilitySystemComponent->AddLooseGameplayTag(DeathBlowedTag);
}

void AEnemyCharacterBase::HandleEndDeathBlow()
{
	if (MotionWarpingComponent)
	{
		MotionWarpingComponent->RemoveWarpTarget(FName("JumpToInstigator"));
	}

	AbilitySystemComponent->RemoveLooseGameplayTag(DeathBlowedTag);
}

void AEnemyCharacterBase::Die()
{
	Super::Die();

	AAA_AIController* AIController = Cast<AAA_AIController>(GetController());
	if (AIController)
	{
		AIController->LostPlayer();
		UnPossessedAIController = AIController;
		AIController->UnPossess();
	}

	if (FloatingStatusBarWidgetComp)
	{
		FloatingStatusBarWidgetComp->SetVisibility(false);
	}
}

void AEnemyCharacterBase::SetLockOnTrackerVisibility(bool InState)
{
	if (InState)
	{
		LockOnWidgetComp->SetVisibility(true);
	}
	else
	{
		LockOnWidgetComp->SetVisibility(false);
	}
}

void AEnemyCharacterBase::SetDeathBlowTrackerVisibility(bool InState)
{
	if (InState)
	{
		DeathBlowWidgetComp->SetVisibility(true);
	}
	else
	{
		DeathBlowWidgetComp->SetVisibility(false);
	}
}

void AEnemyCharacterBase::SetBackstabDeathBlowTrackerVisibility(bool InState)
{
	if (InState)
	{
		BackstabDeathBlowWidgetComp->SetVisibility(true);
	}
	else
	{
		BackstabDeathBlowWidgetComp->SetVisibility(false);
	}
}

void AEnemyCharacterBase::SetPlungeDeathBlowTrackerVisibility(bool InState)
{
	if (InState)
	{
		PlungeDeathBlowWidgetComp->SetVisibility(true);
	}
	else
	{
		PlungeDeathBlowWidgetComp->SetVisibility(false);
	}
}

void AEnemyCharacterBase::SetStatusBarVisibility()
{
	if (bIsLockedOn || bIsOverlappingUISphere)
	{
		FloatingStatusBarWidgetComp->SetVisibility(true);
	}
	else
	{
		FloatingStatusBarWidgetComp->SetVisibility(false);
	}
	
}

bool AEnemyCharacterBase::CanBeBackstabbed(const AActor* OtherActor)
{
	AAA_AIController* AIController = Cast<AAA_AIController>(GetController());
	if (AIController)
	{
		if (AIController->GetTarget() != nullptr)
		{
			return false;
		}
	}

	float DistanceToFrontBackPlane = FVector::PointPlaneDist(OtherActor->GetActorLocation(), GetActorLocation(), GetActorRightVector());
	float DistanceToRightLeftPlane = FVector::PointPlaneDist(OtherActor->GetActorLocation(), GetActorLocation(), GetActorForwardVector());
	if (FMath::Abs(DistanceToFrontBackPlane) <= FMath::Abs(DistanceToRightLeftPlane))
	{
		if (DistanceToRightLeftPlane >= 0)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}

void AEnemyCharacterBase::HandlePoiseDamage(float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags, AAA_CharacterBase* InstigatorCharacter, AActor* DamageCauser)
{
	Super::HandlePoiseDamage(DamageAmount, HitInfo, DamageTags, InstigatorCharacter, DamageCauser);

	// 적이 공격을 GuardCount만큼 막으면 다음 공격은 튕겨낸다.
	if (GuardCheck(DamageCauser) && !IsDeflecting())
	{
		GuardCount++;
		if (GuardCount >= MaxGuardCount)
		{
			bCanDeflect = true;
			GuardCount = 0;
		}
		SetGuardCountTimer();
	}

	if (IsDeflecting())
	{
		bCounter = true;
	}
}

void AEnemyCharacterBase::OnStaminaDepleted()
{
	AbilitySystemComponent->AddLooseGameplayTag(GroggyTag);

	SetDeathBlowTrackerVisibility(true);
}

void AEnemyCharacterBase::OnGroggyMontageComplete()
{
	Super::OnGroggyMontageComplete();

	SetDeathBlowTrackerVisibility(false);
}

void AEnemyCharacterBase::OnDeflectMontageComplete()
{
	Super::OnDeflectMontageComplete();
	//if (bDeflecting)
	//{
	//	// bDeflect = true;
	//	bDeflecting = false;
	//	GetWorldTimerManager().SetTimer(DeflectStateTimerHandle, FTimerDelegate::CreateLambda([this]()
	//		{
	//			bCounter = false;
	//		}), 1.f, false);
	//}
}

void AEnemyCharacterBase::SetGuardCountTimer()
{
	if (GetWorldTimerManager().IsTimerPending(GuardCountTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(GuardCountTimerHandle);
	}

	GetWorldTimerManager().SetTimer(GuardCountTimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			GuardCount = 0;
		}), 2.f, false);
}

void AEnemyCharacterBase::TryActiveGuardAbility()
{
	if (AbilitySystemComponent)
	{
		AAA_AIController* AIController = Cast<AAA_AIController>(GetController());
		if (AIController)
		{
			if (!bCounter)
			{
				// 일정 범위 내에서 플레이어가 공격했을 경우 자동적으로 방어한다.
				if (FVector::Dist(AIController->GetTarget()->GetActorLocation(), GetActorLocation()) < 400.f)
				{
					// AbilitySystemComponent->CancelAbilities(&GuardTags);
					// 일정 카운트 이상 가드에 성공 시, 공격을 튕겨낸다.
					if (bCanDeflect)
					{
						FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
						if (DeflectEffect.Get())
						{
							FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DeflectEffect, 0, EffectContext);
							FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
							if (!ActiveGEHandle.WasSuccessfullyApplied())
							{
								UE_LOG(LogTemp, Error, TEXT("Failed to apply startup effect! %s"), *GetNameSafe(DeflectEffect));
							}
						}

						bCanDeflect = false;

						return;
					}

					if (AbilitySystemComponent->TryActivateAbilitiesByTag(GuardTags, true))
					{
						

						if (GetWorldTimerManager().IsTimerPending(GuardTimerHandle))
						{
							GetWorldTimerManager().ClearTimer(GuardTimerHandle);
						}
						GetWorldTimerManager().SetTimer(GuardTimerHandle, FTimerDelegate::CreateLambda([this]()
							{
								AbilitySystemComponent->CancelAbilities(&GuardTags);
							}), 0.5f, false);

						
					}
				}
			}
			
		}
	}
}

FVector AEnemyCharacterBase::GetLockOnLocation()
{
	return GetMesh()->GetSocketLocation(FName("LockOnLocation"));
}

void AEnemyCharacterBase::InitializeFloatingStatusBar()
{
	if (FloatingStatusBar || !AbilitySystemComponent)
	{
		return;
	}

	AAA_AIController* AIController = Cast<AAA_AIController>(GetController());
	if (AIController)
	{
		if (FloatingStatusBarClass)
		{
			FloatingStatusBar = CreateWidget<UFloatingStatusBarWidget>(AIController->GetWorld(), FloatingStatusBarClass);
			if (FloatingStatusBar && FloatingStatusBarWidgetComp)
			{
				FloatingStatusBarWidgetComp->SetWidget(FloatingStatusBar);

				if (AttributeSet)
				{
					FloatingStatusBar->SetHealthPercentage(AttributeSet->GetHealth() / AttributeSet->GetMaxHealth());
					FloatingStatusBar->SetStaminaPercentage(AttributeSet->GetStamina() / AttributeSet->GetMaxStamina());
					FloatingStatusBar->SetDeathCountNumber(AttributeSet->GetMaxDeathCount());

				}
			}
		}
	}
}

void AEnemyCharacterBase::OnRestInCheckPoint()
{
	if (bResetOnRest)
	{
		ResetCharacterToOrigin();
	}
	else
	{
		Destroy();
		Weapon->Destroy();
	}
}

void AEnemyCharacterBase::ResetCharacterToOrigin()
{
	if (AbilitySystemComponent)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(ResetAttributeEffect, 1, EffectContext);
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			if (!ActiveGEHandle.WasSuccessfullyApplied())
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to apply reset effect! %s"), *GetNameSafe(ResetAttributeEffect));
			}
		}

		AbilitySystemComponent->CancelAbilities();

		AbilitySystemComponent->RemoveLooseGameplayTag(DeadTag);
		AbilitySystemComponent->RemoveLooseGameplayTag(DeathBlowedTag);
		AbilitySystemComponent->RemoveLooseGameplayTag(GroggyTag);
	}

	if (UnPossessedAIController)
	{
		UnPossessedAIController->Possess(this);
		PossessedBy(UnPossessedAIController);
		GetController()->Reset();
		UnPossessedAIController = nullptr;
	}

	StopAnimMontage();

	SetActorLocation(ResetTransform.GetLocation());
	SetActorRotation(ResetTransform.GetRotation());

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->GravityScale = 1.2f;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

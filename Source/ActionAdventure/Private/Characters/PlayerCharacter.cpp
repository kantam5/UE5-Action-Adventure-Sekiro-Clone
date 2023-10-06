// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/SphereComponent.h"
#include "AbilitySystem/Components/AA_AbilitySystemComponent.h"
#include "ActorComponents/AbilityTraversalComponent.h"
#include "ActorComponents/AA_CharacterMovementComponent.h"
#include "ActorComponents/GrapplingHookComponent.h"
#include "AbilitySystem/Abilities/AA_GameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbility.h"
#include "Kismet/KismetMathLibrary.h"
#include "AnimNotifies/ANS_JumpSection.h"
#include "AnimInstances/AA_AnimInstance.h"
#include "Characters/EnemyCharacterBase.h"
#include "DataAssets/CharacterAnimDataAsset.h"
#include "Actors/HookablePoint.h"
#include "AbilitySystem/AttributeSets/AA_AttributeSetBase.h"
#include "CableComponent.h"
#include "Characters/MinionCharacter.h"
#include "Player/AA_PlayerController.h"
#include "ActorComponents/InteractionComponent.h"
#include "DataAssets/AA_Item.h"
#include "Kismet/GameplayStatics.h"
#include "AA_GameTypes.h"
#include "GameInstances/AA_GameInstance.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UAA_CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
	, SlottedSkills()
	, CurrentSkillIndex(0)
	, bHasFilledSkillSlot(false)
	, SlottedPotions()
	, CurrentPotionIndex(0)
	, bHasFilledPotionSlot(false)
	, bIsLockOn(false)
	, bCanBackstab(false)
	, bEnableComboPeriod(false)
	, DeflectLevel(0.f)
	, bDeflectTimerPending(false)
	, HookHittedLoc(FVector())
	, bHookHit(false)
	, bCanFinish(false)
	, bInventoryUsing(false)
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; 	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.7f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 80.f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false; 

	PlungingDeathBlowCollision = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("PlungingDeathBlowCollision"));
	PlungingDeathBlowCollision->SetupAttachment(RootComponent);
	PlungingDeathBlowCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlungingDeathBlowCollision->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::PlungeSphereCollisionBeginOverlap);
	
	BackstabDeathBlowCollision = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("BackstabDeathBlowCollision"));
	BackstabDeathBlowCollision->SetupAttachment(RootComponent);
	BackstabDeathBlowCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	StatusBarCollision = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("StatusBarCollision"));
	StatusBarCollision->SetupAttachment(RootComponent);
	StatusBarCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StatusBarCollision->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::StatusBarSphereCollisionBeginOverlap);
	StatusBarCollision->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::StatusBarSphereCollisionEndOverlap);

	InteractionCollision = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("InteractionCollision"));
	InteractionCollision->SetupAttachment(RootComponent);
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	/*InteractionCollision->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::StatusBarSphereCollisionBeginOverlap);
	InteractionCollision->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::StatusBarSphereCollisionEndOverlap);*/

	AbilityTraversal = CreateDefaultSubobject<UAbilityTraversalComponent>(TEXT("AbilityTraversal"));

	GrapplingHookComponent = CreateDefaultSubobject<UGrapplingHookComponent>(TEXT("GrapplingHook"));

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

	Hook = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hook"));
	Hook->SetupAttachment(GetMesh(), "Idle_Hook_Socket");
	Hook->SetEnableGravity(false);

	Rope = CreateDefaultSubobject<UCableComponent>(TEXT("Rope Cable"));
	Rope->SetupAttachment(GetMesh(), "Rope_Socket");

	WeaponSocket = TEXT("Weapon");

	bIsArmed = true;

	TeamNumber = 0;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(GrapplingHookComponent, &UGrapplingHookComponent::OnCapsuleOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(GrapplingHookComponent, &UGrapplingHookComponent::OnCapsuleOverlapEnd);

	InteractionCollision->OnComponentBeginOverlap.AddDynamic(InteractionComponent, &UInteractionComponent::OnCollisionOverlapBegin);
	InteractionCollision->OnComponentEndOverlap.AddDynamic(InteractionComponent, &UInteractionComponent::OnCollisionOverlapEnd);

	UWorld* World = GetWorld();
	UAA_GameInstance* GameInstance = World ? World->GetGameInstance<UAA_GameInstance>() : nullptr;
	if (GameInstance)
	{
		GameInstance->OnRestInCheckPoint.AddDynamic(this, &APlayerCharacter::OnRestInCheckPoint);
	}

	if (CloseCameraCurve != nullptr && OutCameraCurve != nullptr)
	{
		FOnTimelineFloat CloseCameraCurveCallback;
		FOnTimelineFloat OutCameraCurveCallback;

		CloseCameraCurveCallback.BindUFunction(this, FName("CloseCameraTimelineProgress"));
		OutCameraCurveCallback.BindUFunction(this, FName("OutCameraTimelineProgress"));

		CloseCameraTimeline.AddInterpFloat(CloseCameraCurve, CloseCameraCurveCallback);
		OutCameraTimeline.AddInterpFloat(OutCameraCurve, OutCameraCurveCallback);

		CloseCameraTimeline.SetTimelineLength(0.2f);
		OutCameraTimeline.SetTimelineLength(0.2f);
	}
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// TODO: add attack state
	if (bIsLockOn && LockOnTarget)
	{
		if (LockOnCurve->IsValidLowLevel())
		{
			FRotator CurrentControllerRotation = GetController()->GetControlRotation();
			AEnemyCharacterBase* LockOnEnemy = Cast<AEnemyCharacterBase>(LockOnTarget);
			FVector TargetLocation = LockOnEnemy->GetLockOnLocation();
			// FVector TargetLocation = LockOnTarget->GetActorLocation();
			// FVector TargetLocation = LockOnTarget->GetMesh()->GetComponentLocation();
			float Distance = LockOnCurve->GetFloatValue(FVector::Dist(GetActorLocation(), LockOnTarget->GetActorLocation()));
			TargetLocation.Z -= Distance;
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(FollowCamera->GetComponentLocation(), TargetLocation);
			FRotator InterpolateRotation = FMath::RInterpTo(CurrentControllerRotation, LookAtRotation, DeltaSeconds, 40.f);
			FRotator NewRotation = FRotator(InterpolateRotation.Pitch, InterpolateRotation.Yaw, CurrentControllerRotation.Roll);
			GetController()->SetControlRotation(NewRotation);
		}
	}


	if (GetCharacterMovement()->IsFalling())
	{
		if (AbilitySystemComponent)
		{
			if (!AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.InAir"))))
			{
				AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.InAir")));
			}
		}
	}

	SetPlungeEnemy();

	SetBackstabEnemy();

	if (CloseCameraTimeline.IsPlaying())
	{
		CloseCameraTimeline.TickTimeline(DeltaSeconds);
	}
	if (OutCameraTimeline.IsPlaying())
	{
		OutCameraTimeline.TickTimeline(DeltaSeconds);
	}
	
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InventorySource = NewController;

	if (InventorySource)
	{
		InventoryUpdateHandle = InventorySource->GetSlottedItemChangedDelegate().AddUObject(this, &APlayerCharacter::OnItemSlotChanged);
		// TODO: Load Delegate
	}

	// 슬롯으로 장착한 아이템의 어빌리티 획득
	if (AbilitySystemComponent)
	{
		AddSlottedGameplayAbilities();
	}
}

void APlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveActiveEffectsWithTags(InAirTags);
		AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.InAir")), INT_MAX);

		FGameplayTagContainer TagContainer(FGameplayTag::RequestGameplayTag("Ability.Melee"));
		AbilitySystemComponent->CancelAbilities(&TagContainer);
	}
}

void APlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	if (!CrouchStateEffect.Get())
	{
		return;
	}

	if (AbilitySystemComponent)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(CrouchStateEffect, 1, EffectContext);
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			if (!ActiveGEHandle.WasSuccessfullyApplied())
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to apply crouch effect! %s"), *GetNameSafe(CrouchStateEffect));
			}
		}
	}
}

void APlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (AbilitySystemComponent && CrouchStateEffect.Get())
	{
		AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(CrouchStateEffect, AbilitySystemComponent);

		CameraBoom->SetRelativeLocation(CameraBoom->GetRelativeLocation());
	}

	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

}

void APlayerCharacter::HandlePoiseDamage(float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags, AAA_CharacterBase* InstigatorCharacter, AActor* DamageCauser)
{
	Super::HandlePoiseDamage(DamageAmount, HitInfo, DamageTags, InstigatorCharacter, DamageCauser);

	// 튕겨내기 시, 적에게 스태미나 데미지
	if (DeflectCheck(InstigatorCharacter))
	{
		FGameplayEffectContextHandle ContextHandle;
		ContextHandle.AddInstigator(this, Cast<AActor>(Weapon));
		ContextHandle.AddSourceObject(this);
		InstigatorCharacter->ApplyGameplayEffectToSelf(DeflectEffect, ContextHandle);
	}
}

bool APlayerCharacter::IsUsingMelee() const
{
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer GameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("Ability.Melee")));
		TArray<UAA_GameplayAbility*> ActiveAbilities;
		AbilitySystemComponent->GetActiveAbilitiesWithTags(GameplayTagContainer, ActiveAbilities);
		
		return ActiveAbilities.Num() > 0;
	}

	return false;
}

void APlayerCharacter::JumpSectionForComboServer_Implementation()
{
	JumpSectionForComboNetMulticast();
}

void APlayerCharacter::JumpSectionForComboNetMulticast_Implementation()
{
	if (!IsValid(JumpSectionNotify) || !bEnableComboPeriod)
	{
		return;
	}

	if (AnimInstance)
	{
		UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
		FName CurrentSection = AnimInstance->Montage_GetCurrentSection(CurrentMontage);
		FName NextSection = JumpSectionNotify->JumpSection;
		AnimInstance->Montage_JumpToSection(NextSection, CurrentMontage);

		SetEnableComboPeriod(false);
	}

	
}

void APlayerCharacter::CloseCameraTimelineProgress(float Value)
{
	CameraBoom->TargetArmLength = Value;
}

void APlayerCharacter::OutCameraTimelineProgress(float Value)
{
	CameraBoom->TargetArmLength = Value;
}

void APlayerCharacter::OnHealthDepleted()
{
	AttributeSet->SetDeathCount(0.f);
	AnimInstance->Montage_Play(CharacterData.CharacterAnimDataAsset->CharacterAnimationData.DeathMontage);

	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(Controller);
	DisableInput(PlayerController);
	// Die();
}

void APlayerCharacter::OnStaminaDepleted()
{
	AbilitySystemComponent->AddLooseGameplayTag(GroggyTag);

	if (AnimInstance->IsValidLowLevel())
	{
		AbilitySystemComponent->CancelAbilities(&GuardTags);
		AnimInstance->PlayGroggyMontage();
	}
}

void APlayerCharacter::Die()
{
	Super::Die();

	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(Controller);
	if (PlayerController->IsValidLowLevel())
	{
		PlayerController->OnPlayerDied();
	}

}

void APlayerCharacter::OnTargetDeath(AAA_CharacterBase* Character)
{
	if (!IsValid(Character))
	{
		return;
	}
	
	CancelLockOn();
}

void APlayerCharacter::CancelLockOn()
{
	AbilitySystemComponent->CancelAbilities(&LockOnTags);
}

void APlayerCharacter::SetDeflectTimer()
{
	bDeflectTimerPending = true;
	if (GetWorldTimerManager().IsTimerPending(DeflectTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(DeflectTimerHandle);
	}

	GetWorldTimerManager().SetTimer(DeflectTimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			bDeflectTimerPending = false;
			DeflectLevel = 0.f;
		}), 0.5f, false);

}

void APlayerCharacter::PlungeSphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != LockOnTarget)
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("OtherComp: %s"), *OtherComp->GetName());
	AEnemyCharacterBase* EnemyCharacter = Cast<AEnemyCharacterBase>(OtherActor);
	if (IsValid(EnemyCharacter))
	{
		FGameplayTag MontageTag = FGameplayTag::RequestGameplayTag(FName("Event.Montage.DeathBlow.Plunging"));
		UAnimMontage* MontageToPlay = *CharacterData.CharacterAnimDataAsset->CharacterAnimationData.DeathBlowMontageMap.Find(MontageTag);
		AnimInstance->Montage_Play(MontageToPlay);

		EnemyCharacter->HandleDeathBlow(MontageTag, this);

		EndPlungeCollisionOverlap();

	}
}

void APlayerCharacter::BeginPlungeCollisionOverlap()
{
	PlungingDeathBlowCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void APlayerCharacter::EndPlungeCollisionOverlap()
{
	PlungingDeathBlowCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APlayerCharacter::SetBackstabEnemy()
{
	TArray<AActor*> OverlappingEnemies;
	BackstabDeathBlowCollision->GetOverlappingActors(OverlappingEnemies, AEnemyCharacterBase::StaticClass());
	if (OverlappingEnemies.Num() > 0)
	{
		AActor* MinDistEnemy = OverlappingEnemies[0];
		for (AActor* Enemy : OverlappingEnemies)
		{
			if (FVector::Dist(GetActorLocation(), Enemy->GetActorLocation()) < FVector::Dist(GetActorLocation(), MinDistEnemy->GetActorLocation()))
			{
				MinDistEnemy = Enemy;
			}
		}

		if (BackstabEnemy != Cast<AEnemyCharacterBase>(MinDistEnemy))
		{
			if (IsValid(BackstabEnemy))
			{
				BackstabEnemy->SetBackstabDeathBlowTrackerVisibility(false);
			}
		}

		BackstabEnemy = Cast<AEnemyCharacterBase>(MinDistEnemy);
		if (IsValid(BackstabEnemy))
		{
			if (BackstabEnemy->CanBeBackstabbed(this) && BackstabEnemy->IsAlive())
			{
				BackstabEnemy->SetBackstabDeathBlowTrackerVisibility(true);
				bCanBackstab = true;
			}
			else
			{
				BackstabEnemy->SetBackstabDeathBlowTrackerVisibility(false);
				BackstabEnemy = nullptr;
				bCanBackstab = false;
			}
		}
		else
		{
			BackstabEnemy = nullptr;
			bCanBackstab = false;
		}
	}
	else
	{
		if (IsValid(BackstabEnemy))
		{
			BackstabEnemy->SetBackstabDeathBlowTrackerVisibility(false);
		}
		BackstabEnemy = nullptr;
		bCanBackstab = false;
	}
}

void APlayerCharacter::SetPlungeEnemy()
{
	if (PlungeEnemy != Cast<AEnemyCharacterBase>(LockOnTarget))
	{
		if (IsValid(PlungeEnemy))
		{
			PlungeEnemy->SetPlungeDeathBlowTrackerVisibility(false);
		}
	}

	PlungeEnemy = Cast<AEnemyCharacterBase>(LockOnTarget);
	if (IsValid(PlungeEnemy))
	{
		FVector DistVector = GetActorLocation() - LockOnTarget->GetActorLocation();
		if (FMath::Abs(DistVector.Z) < 1000.f)
		{
			PlungeEnemy->SetPlungeDeathBlowTrackerVisibility(false);
			PlungeEnemy = nullptr;
			return;
		}

		FVector XYDistVector = FVector(DistVector.X, DistVector.Y, 0.f);
		if (XYDistVector.Size() > 1000.f)
		{
			PlungeEnemy->SetPlungeDeathBlowTrackerVisibility(false);
			PlungeEnemy = nullptr;
			return;
		}

		FCollisionQueryParams TraceParams(TEXT("PlungeTrace"), false);
		TraceParams.bReturnPhysicalMaterial = false;

		FHitResult HitDetails = FHitResult(ForceInit);

		bool IsHit = GetWorld()->LineTraceSingleByChannel(
			HitDetails,
			PlungeEnemy->GetActorLocation(),
			GetActorLocation(),
			ECC_Visibility,
			TraceParams);

		if (IsHit)
		{
			PlungeEnemy->SetPlungeDeathBlowTrackerVisibility(false);
			PlungeEnemy = nullptr;
			return;
		}

		PlungeEnemy->SetPlungeDeathBlowTrackerVisibility(true);
	}
	else
	{
		PlungeEnemy = nullptr;
	}
}

void APlayerCharacter::StatusBarSphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemyCharacterBase* EnemyCharacter = Cast<AEnemyCharacterBase>(OtherActor);
	if (IsValid(EnemyCharacter))
	{
		EnemyCharacter->SetIsOverlappingUISphere(true);
	}
}

void APlayerCharacter::StatusBarSphereCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AEnemyCharacterBase* EnemyCharacter = Cast<AEnemyCharacterBase>(OtherActor);
	if (IsValid(EnemyCharacter))
	{
		EnemyCharacter->SetIsOverlappingUISphere(false);
	}
}

void APlayerCharacter::SetUpperBodyMontage(bool InState)
{
	if (AnimInstance)
	{
		AnimInstance->SetUpperBody(InState);
	}
}

void APlayerCharacter::OnRestInCheckPoint()
{
	StopAnimMontage();

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
		AbilitySystemComponent->RemoveLooseGameplayTag(GroggyTag);
	}

	AttributeSet->SetDeathCount(1.f);

	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(Controller);
	EnableInput(PlayerController);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->GravityScale = 1.2f;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool APlayerCharacter::CanHookThrow() const
{
	return GrapplingHookComponent->CanHookThrow();
}

UAnimMontage* APlayerCharacter::GetHookMontage()
{
	return GrapplingHookComponent->GetHookMontage();
}

void APlayerCharacter::LerpHookCharacterMovement(const FVector& StartLoc, const FVector& EndLoc, float Time)
{
	if (GrapplingHookComponent->GetCurrentMontage())
	{
		if (GrapplingHookComponent->GetCurrentMontage()->HeightCurve->IsValidLowLevel() &&
			GrapplingHookComponent->GetCurrentMontage()->SpeedCurve->IsValidLowLevel())
		{
			FVector LerpLoc = UKismetMathLibrary::VLerp(StartLoc, EndLoc,
				GrapplingHookComponent->GetCurrentMontage()->SpeedCurve->GetFloatValue(Time));

			LerpLoc.Z += GrapplingHookComponent->GetCurrentMontage()->HeightCurve->GetFloatValue(Time);

			SetActorLocation(LerpLoc);
		}
	}
}

void APlayerCharacter::AttachHook()
{
	Hook->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "Idle_Hook_Socket");
}

void APlayerCharacter::LerpHookTransform(const FTransform& StartTrans, const FTransform& EndTrans, float Time)
{
	if (GrapplingHookComponent->GetCurrentMontage())
	{
		if (GrapplingHookComponent->GetCurrentMontage()->SwingCurve->IsValidLowLevel())
		{
			FTransform LerpTransform = UKismetMathLibrary::TLerp(StartTrans, EndTrans,
				GrapplingHookComponent->GetCurrentMontage()->SwingCurve->GetFloatValue(Time));

			Hook->SetWorldTransform(LerpTransform);
		}
	}
}

void APlayerCharacter::LerpHookRotation(const FRotator& StartRot, const FRotator& EndRot, float Alpha)
{
	FRotator LerpRot = UKismetMathLibrary::RLerp(StartRot, EndRot, Alpha, false);

	Hook->SetWorldRotation(LerpRot);
}

void APlayerCharacter::HookThrow(const FVector& StartLoc, const FVector& EndLoc, float Time, float NextFrameTime)
{
	if (GrapplingHookComponent->GetCurrentMontage())
	{
		if (GrapplingHookComponent->GetCurrentMontage()->ThrowSpeedCurve->IsValidLowLevel())
		{
			if (Time != 0.f)
			{
				Hook->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			}

			// 훅을 던져서 프레임마다 다음 위치를 확인해서 충돌이 발생했다면 훅을 고정한다.
			if (!bHookHit)
			{
				FVector LerpLoc = UKismetMathLibrary::VLerp(StartLoc, EndLoc,
					GrapplingHookComponent->GetCurrentMontage()->ThrowSpeedCurve->GetFloatValue(Time));
				
				Hook->SetWorldLocation(LerpLoc);

				FVector NextFrameLerpLoc = UKismetMathLibrary::VLerp(StartLoc, EndLoc,
					GrapplingHookComponent->GetCurrentMontage()->ThrowSpeedCurve->GetFloatValue(NextFrameTime));

				CheckHookCollision(NextFrameLerpLoc);
			}
			else
			{
				Hook->SetWorldLocation(HookHittedLoc);
			}
		}
	}
}

void APlayerCharacter::HookPickUp(const FVector& StartLoc, const FVector& EndLoc, float Time)
{
	if (GrapplingHookComponent->GetCurrentMontage())
	{
		bHookHit = false;

		if (GrapplingHookComponent->GetCurrentMontage()->PickupSpeedCurve->IsValidLowLevel())
		{
			if (Time >= 0.25)
			{
				FAttachmentTransformRules RotAttachment = FAttachmentTransformRules(
					EAttachmentRule::KeepWorld, EAttachmentRule::SnapToTarget,
					EAttachmentRule::KeepWorld, true);
				Hook->AttachToComponent(GetMesh(), RotAttachment, "Idle_Hook_Socket");
			}

			FVector LerpLoc = UKismetMathLibrary::VLerp(StartLoc, EndLoc,
				GrapplingHookComponent->GetCurrentMontage()->PickupSpeedCurve->GetFloatValue(Time));

			Hook->SetWorldLocation(LerpLoc);
		}
	}
}

bool APlayerCharacter::CheckRopeVisibility() const
{
	if (Hook->GetVisibleFlag() || Rope->GetVisibleFlag())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void APlayerCharacter::ChangeRopeVisibility(bool InState)
{
	if (Rope && Hook)
	{
		Rope->SetVisibility(InState);
		Hook->SetVisibility(InState);
	}
}

void APlayerCharacter::SetRopeStiffness(bool InState)
{
	if (Rope)
	{
		Rope->bEnableStiffness = InState;
	}
}

void APlayerCharacter::UpdateRopeLength(float Time)
{
	if (GrapplingHookComponent->GetCurrentMontage())
	{
		if (GrapplingHookComponent->GetCurrentMontage()->LengthCurve->IsValidLowLevel() &&
			GrapplingHookComponent->GetCurrentMontage()->SolverIterationsCurve->IsValidLowLevel())
		{
			Rope->CableLength = GrapplingHookComponent->GetCurrentMontage()->LengthCurve->GetFloatValue(Time);
			Rope->SolverIterations = (int)GrapplingHookComponent->GetCurrentMontage()->SolverIterationsCurve->GetFloatValue(Time);
		}
	}
}

void APlayerCharacter::CheckHookCollision(const FVector& NextFrameLoc)
{
	if (Hook->IsValidLowLevel())
	{
		FCollisionQueryParams TraceParams(TEXT("HookCollisionTrace"), false);
		TraceParams.bReturnPhysicalMaterial = false;

		FHitResult HitDetails = FHitResult(ForceInit);

		bool IsHit = GetWorld()->LineTraceSingleByChannel(
			HitDetails,
			Hook->GetComponentLocation(),
			NextFrameLoc,
			ECC_GameTraceChannel3,
			TraceParams);
		
		if (IsHit)
		{
			bHookHit = true;
			
			FVector MinHookBounds = FVector::ZeroVector;
			FVector MaxHookBounds = FVector::ZeroVector;
			Hook->GetLocalBounds(MinHookBounds, MaxHookBounds);

			HookHittedLoc = HitDetails.Location - (MaxHookBounds * 0.5f);
		}
	}
}

void APlayerCharacter::SetFinishDeathBlowState(ABossCharacter* InBoss)
{
	Boss = InBoss;
	bCanFinish = true;
	PlayAnimMontage(FinisherDeathBlowPrepMontage);

	if (!AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.BlockInput"))))
	{
		AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.BlockInput")));
	}
	
}

void APlayerCharacter::EndFinisherDeathBlow()
{
	Boss = nullptr;
	bCanFinish = false;
	StopAnimMontage(FinisherDeathBlowPrepMontage);

	if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.BlockInput"))))
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.BlockInput")));
	}

	OutCameraTimeline.PlayFromStart();
}

bool APlayerCharacter::ActiveAbilityWithItem(UAA_Item* InItem)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(InItem->GrangedAbility));
		
		return AbilitySystemComponent->TryActivateAbilityByClass(InItem->GrangedAbility);
	}

	return false;
}

// 슬롯 아이템이 변경되었을 때, 호출
void APlayerCharacter::OnItemSlotChanged(FAA_ItemSlot ItemSlot, UAA_Item* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("OnItemSlotChanged"));
	RefreshSlottedGameplayAbilities();
	RefreshSlottedItems();
}

// 슬롯에 있는 어빌리티에 따라, 캐릭터의 어빌리티를 변경한다.
void APlayerCharacter::RefreshSlottedGameplayAbilities()
{
	RemoveSlottedGameplayAbilities(false);
	AddSlottedGameplayAbilities();
}

void APlayerCharacter::RefreshSlottedItems()
{
	SlottedSkills.Empty();
	SlottedPotions.Empty();

	bool HasFilledSkillSlot = false;
	bool HasFilledPotionSlot = false;

	if (InventorySource)
	{
		const TMap<FAA_ItemSlot, UAA_Item*>& SlottedItemMap = InventorySource->GetSlottedItemMap();
		for (const TPair<FAA_ItemSlot, UAA_Item*>& ItemPair : SlottedItemMap)
		{
			UAA_Item* SlottedItem = ItemPair.Value;

			if (ItemPair.Key.ItemType == UAA_AssetManager::SkillItemType)
			{
				SlottedSkills.Add(SlottedItem);
				if (SlottedItem != nullptr)
				{
					HasFilledSkillSlot = true;
				}
			}
			else if (ItemPair.Key.ItemType == UAA_AssetManager::PotionItemType)
			{
				SlottedPotions.Add(SlottedItem);
				if (SlottedItem != nullptr)
				{
					HasFilledPotionSlot = true;
				}
			}
		}
	}

	bHasFilledSkillSlot = HasFilledSkillSlot;
	bHasFilledPotionSlot = HasFilledPotionSlot;

}

// 슬롯에 장착한 아이템의 어빌리티 획득
void APlayerCharacter::AddSlottedGameplayAbilities()
{
	TMap<FAA_ItemSlot, FGameplayAbilitySpec> SlottedAbilitySpec;
	FillSlottedAbilitySpecs(SlottedAbilitySpec);

	for (const TPair<FAA_ItemSlot, FGameplayAbilitySpec>& SpecPair : SlottedAbilitySpec)
	{
		FGameplayAbilitySpecHandle& SpecHandle = SlottedAbilities.FindOrAdd(SpecPair.Key);

		if (!SpecHandle.IsValid())
		{
			SpecHandle = AbilitySystemComponent->GiveAbility(SpecPair.Value);
		}
	}
}

// SlottedAbilitySpecs를 슬롯에 장착한 아이템으로 채운다.
void APlayerCharacter::FillSlottedAbilitySpecs(TMap<FAA_ItemSlot, FGameplayAbilitySpec>& SlottedAbilitySpecs)
{
	for (const TPair<FAA_ItemSlot, TSubclassOf<UAA_GameplayAbility>>& DefaultPair : DefaultSlottedAbilities)
	{
		if (DefaultPair.Value.Get())
		{
			SlottedAbilitySpecs.Add(DefaultPair.Key, FGameplayAbilitySpec(DefaultPair.Value, 1, INDEX_NONE, this));
		}
	}

	if (InventorySource)
	{
		const TMap<FAA_ItemSlot, UAA_Item*>& SlottedItemMap = InventorySource->GetSlottedItemMap();
		for (const TPair<FAA_ItemSlot, UAA_Item*>& ItemPair : SlottedItemMap)
		{
			UAA_Item* SlottedItem = ItemPair.Value;

			if (SlottedItem && SlottedItem->GrangedAbility)
			{
				SlottedAbilitySpecs.Add(ItemPair.Key, FGameplayAbilitySpec(SlottedItem->GrangedAbility, 1, INDEX_NONE, SlottedItem));
			}
		}
	}
}

// 현재 장착중인 아이템과 비교하여, 획득한 어빌리티를 수정한다.
void APlayerCharacter::RemoveSlottedGameplayAbilities(bool bRemoveAll)
{
	TMap<FAA_ItemSlot, FGameplayAbilitySpec> SlottedAbilitySpecs;

	if (!bRemoveAll)
	{
		FillSlottedAbilitySpecs(SlottedAbilitySpecs);
	}

	for (TPair<FAA_ItemSlot, FGameplayAbilitySpecHandle>& ExistingPair : SlottedAbilities)
	{
		FGameplayAbilitySpec* FoundSpec = AbilitySystemComponent->FindAbilitySpecFromHandle(ExistingPair.Value);
		bool bShouldRemove = bRemoveAll || !FoundSpec;

		if (!bShouldRemove)
		{
			FGameplayAbilitySpec* DesiredSpec = SlottedAbilitySpecs.Find(ExistingPair.Key);
			if (!DesiredSpec || DesiredSpec->Ability != FoundSpec->Ability || DesiredSpec->SourceObject != FoundSpec->SourceObject)
			{
				bShouldRemove = true;
			}
		}

		if (bShouldRemove)
		{
			if (FoundSpec)
			{
				AbilitySystemComponent->ClearAbility(ExistingPair.Value);
			}

			ExistingPair.Value = FGameplayAbilitySpecHandle();
		}
	}
}


void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Triggered, this, &APlayerCharacter::OnJumpActionStarted);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		EnhancedInputComponent->BindAction(CrouchInputAction, ETriggerEvent::Triggered, this, &APlayerCharacter::OnCrouchActionStarted);
	
		EnhancedInputComponent->BindAction(SprintInputAction, ETriggerEvent::Triggered, this, &APlayerCharacter::OnSprintActionStarted);
		EnhancedInputComponent->BindAction(SprintInputAction, ETriggerEvent::Completed, this, &APlayerCharacter::OnSprintActionEnded);

		EnhancedInputComponent->BindAction(GuardInputAction, ETriggerEvent::Triggered, this, &APlayerCharacter::OnGuardActionStarted);
		EnhancedInputComponent->BindAction(GuardInputAction, ETriggerEvent::Completed, this, &APlayerCharacter::OnGuardActionEnded);

		EnhancedInputComponent->BindAction(LockOnInputAction, ETriggerEvent::Started, this, &APlayerCharacter::OnLockOnActionStarted);
		
		EnhancedInputComponent->BindAction(NormalAttackInputAction, ETriggerEvent::Triggered, this, &APlayerCharacter::OnNormalAttackActionStarted);
	
		EnhancedInputComponent->BindAction(HeavyAttackInputAction, ETriggerEvent::Triggered, this, &APlayerCharacter::OnHeavyAttackActionStarted);
		
		EnhancedInputComponent->BindAction(GrapplingHookInputAction, ETriggerEvent::Triggered, this, &APlayerCharacter::OnGrapplingHookActionStarted);
		
		EnhancedInputComponent->BindAction(InteractInputAction, ETriggerEvent::Triggered, this, &APlayerCharacter::OnInteractActionStarted);
		
		EnhancedInputComponent->BindAction(InventoryInputAction, ETriggerEvent::Triggered, this, &APlayerCharacter::OnInventoryActionStarted);
		
		EnhancedInputComponent->BindAction(SwapSkillAction, ETriggerEvent::Triggered, this, &APlayerCharacter::OnSwapSkilActionStarted);
		EnhancedInputComponent->BindAction(SwapPotionAction, ETriggerEvent::Triggered, this, &APlayerCharacter::OnSwapItemActionStarted);
		
		EnhancedInputComponent->BindAction(UsePotionAction, ETriggerEvent::Triggered, this, &APlayerCharacter::OnUsePotionActionStarted);
		EnhancedInputComponent->BindAction(UseSkillAction, ETriggerEvent::Triggered, this, &APlayerCharacter::OnUseSkillActionStarted);
		EnhancedInputComponent->BindAction(CombatArtAction, ETriggerEvent::Triggered, this, &APlayerCharacter::OnCombatArtActionStarted);
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr && !bIsLockOn)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::OnJumpActionStarted(const FInputActionValue& Value)
{
	if (AbilityTraversal && AbilitySystemComponent)
	{
		AbilityTraversal->TryJumpTraversal(AbilitySystemComponent);
	}
}

void APlayerCharacter::OnCrouchActionStarted(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		if (GetMovementComponent()->IsCrouching())
		{
			AbilitySystemComponent->CancelAbilities(&CrouchTags);
		}
		else
		{
			AbilitySystemComponent->TryActivateAbilitiesByTag(CrouchTags, true);
		}
	}
}

void APlayerCharacter::OnCrouchActionEnded(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAbilities(&CrouchTags);
	}
}

void APlayerCharacter::OnSprintActionStarted(const FInputActionValue& Value)
{
	if (AbilityTraversal && AbilitySystemComponent)
	{
		AbilityTraversal->TryShiftTraversal(AbilitySystemComponent);
	}
}

void APlayerCharacter::OnSprintActionEnded(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAbilities(&SprintTags);
	}
}

void APlayerCharacter::OnGuardActionStarted(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(GuardTags, true);
	}
}

void APlayerCharacter::OnGuardActionEnded(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAbilities(&GuardTags);
	}
}

void APlayerCharacter::OnLockOnActionStarted(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		if (bIsLockOn)
		{
			AbilitySystemComponent->CancelAbilities(&LockOnTags);
		}
		else
		{
			AbilitySystemComponent->TryActivateAbilitiesByTag(LockOnTags, true);
		}
	}
}

void APlayerCharacter::OnNormalAttackActionStarted(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		OnCharacterAttack.Broadcast();

		AbilityTraversal->TryNormalAttackTraversal(AbilitySystemComponent);
		if (IsUsingMelee())
		{
			JumpSectionForComboServer();
		}
		/*else
		{
			AbilityTraversal->TryNormalAttackTraversal(AbilitySystemComponent);
		}*/
	}
}

void APlayerCharacter::OnHeavyAttackActionStarted(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		OnCharacterAttack.Broadcast();
		AbilitySystemComponent->TryActivateAbilitiesByTag(HeavyAttackTags, true);
	}
}

void APlayerCharacter::OnGrapplingHookActionStarted(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer GameplayTagContainer(FGameplayTag::RequestGameplayTag(FName("Ability.Hook")));
		TArray<UAA_GameplayAbility*> ActiveAbilities;
		AbilitySystemComponent->GetActiveAbilitiesWithTags(GameplayTagContainer, ActiveAbilities);
		if (ActiveAbilities.Num() > 0 && !GrapplingHookComponent->IsLastAndCurrentFocusedLocEqual())
		{
			FGameplayTagContainer TagContainer(FGameplayTag::RequestGameplayTag(FName("Ability.Hook")));
			AbilitySystemComponent->CancelAbilities(&TagContainer);
		}
		
		AbilitySystemComponent->TryActivateAbilitiesByTag(GrapplingHookTags, true);
	}
}

void APlayerCharacter::OnInteractActionStarted(const FInputActionValue& Value)
{
	if (InteractionComponent)
	{
		InteractionComponent->TryInteract();
		
	}
}

void APlayerCharacter::OnInventoryActionStarted(const FInputActionValue& Value)
{
	if (bInventoryUsing)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), InventorySound);
		AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(Controller);
		if (PlayerController)
		{
			PlayerController->EndInventoryUI();
		}
	}
	else
	{
		UGameplayStatics::PlaySound2D(GetWorld(), InventorySound);
		AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(Controller);
		if (PlayerController)
		{
			PlayerController->ShowInventoryUI();
		}
	}
}

void APlayerCharacter::OnSwapSkilActionStarted(const FInputActionValue& Value)
{
	UGameplayStatics::PlaySound2D(GetWorld(), InventorySound);

	if (bHasFilledSkillSlot)
	{
		CurrentSkillIndex = (++CurrentSkillIndex) % 3;

		while (SlottedSkills[CurrentSkillIndex] == nullptr)
		{
			CurrentSkillIndex = (++CurrentSkillIndex) % 3;
		}
	}

	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(Controller);
	if (PlayerController)
	{
		PlayerController->UpdateHUDItemSlot();
	}
	
}

void APlayerCharacter::OnSwapItemActionStarted(const FInputActionValue& Value)
{
	UGameplayStatics::PlaySound2D(GetWorld(), InventorySound);

	if (bHasFilledPotionSlot)
	{
		CurrentPotionIndex = (++CurrentPotionIndex) % 5;

		while (SlottedPotions[CurrentPotionIndex] == nullptr)
		{
			CurrentPotionIndex = (++CurrentPotionIndex) % 5;
		}
	}
	
	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(Controller);
	if (PlayerController)
	{
		PlayerController->UpdateHUDItemSlot();
	}

}

void APlayerCharacter::OnUsePotionActionStarted(const FInputActionValue& Value)
{
	FAA_ItemSlot ItemSlot(UAA_AssetManager::PotionItemType, CurrentPotionIndex);

	FGameplayAbilitySpecHandle* FoundHandle = SlottedAbilities.Find(ItemSlot);
	if (FoundHandle && AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbility(*FoundHandle);
	}
}

void APlayerCharacter::OnUseSkillActionStarted(const FInputActionValue& Value)
{
	FAA_ItemSlot ItemSlot(UAA_AssetManager::SkillItemType, CurrentSkillIndex);

	FGameplayAbilitySpecHandle* FoundHandle = SlottedAbilities.Find(ItemSlot);
	if (FoundHandle && AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbility(*FoundHandle);
	}
}

void APlayerCharacter::OnCombatArtActionStarted(const FInputActionValue& Value)
{
	FAA_ItemSlot ItemSlot(UAA_AssetManager::CombatArtItemType, 0);

	FGameplayAbilitySpecHandle* FoundHandle = SlottedAbilities.Find(ItemSlot);
	if (FoundHandle && AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbility(*FoundHandle);
	}
}

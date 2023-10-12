# UE5-Action-Adventure-Sekiro-Clone
Unreal Engine 5.1.1 Portfolio

## 프로젝트 개요

#### 프로젝트 설명  

언리얼 엔진5와 GameplayAbilitySystem을 이용한 3인칭 액션 게임으로 세키로를 모작한 프로젝트입니다.  
위젯을 포함하여 모든 요소는 C++로 작성하였습니다.

#### 엔진 버전  

Unreal Engine 5.1.1

#### 플랫폼

Window 64bit

#### 프로젝트 영상

[![UE5 세키로 모작](https://youtu.be/KjPy1M47Hkk/0.jpg)](https://youtu.be/KjPy1M47Hkk)

#### GAS를 선택한 이유
- GameplayEffect와 GameplayAbility를 GameplayTag로 쉽게 다룰 수 있습니다. 
  - 특정 GameplayEffect나 GameplayAbility를 Tag로 실행시키거나 취소시킬 수 있습니다.
- 새로운 GameplayEffect와 GameplayAbility를 만들어내기 쉽습니다.
  - GameplayEffect로 Attribute의 접근이 편하여 새로운 아이템을 사용했을 때의 GameplayEffect등을 만들기 쉽습니다.
- 캐릭터의 상태를 쉽게 나타낼 수 있습니다.
  - 특정 GameplayTag가 존재한다면 패시브 능력을 멈추거나 Ability를 실행할 수 없도록 제한할 수 있습니다.

## 캐릭터  

#### FCharacterData
- 캐릭터가 가지고 있는 GameplayEffect와 GameplayAbility를 관리합니다.
- 캐릭터가 사용할 애니메이션을관리합니다.
- 이미지
> AA_CharacterTypes.h
``` c++
USTRUCT(BlueprintType)
struct FCharacterData
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
	TArray<TSubclassOf<UGameplayEffect>> Effects;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	UCharacterAnimDataAsset* CharacterAnimDataAsset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HitReact")
	UHitSoundParticleDataAsset* HitSoundParticleDataAsset;
};
```

---
#### AAA_CharacterBase
캐릭터의 베이스가 되는 클래스  

- AbilitySystemComponent을 가지며, GameplayEffect와 GameplayAbility와 상호작용합니다.
- FCharacterData에서 설정한 GameplayEffect와 GameplayAbility를 부여합니다.
- HandlePoiseDamage에서 피격 방향과 공격의 세기에 따라 다른 몽타주를 실행합니다.
> AA_CharacterBase.cpp
``` c++
void AAA_CharacterBase::HandlePoiseDamage(float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags, AAA_CharacterBase* InstigatorCharacter, AActor* DamageCauser)
{
	EHitReactDirection HitReactDirection;

	const FVector& ActorLocation = GetActorLocation();
	float DistanceToFrontBackPlane = FVector::PointPlaneDist(InstigatorCharacter->GetActorLocation(), ActorLocation, GetActorRightVector());
	float DistanceToRightLeftPlane = FVector::PointPlaneDist(InstigatorCharacter->GetActorLocation(), ActorLocation, GetActorForwardVector());
	
	if (DamageAmount > 4.f)
	{
		if (DistanceToRightLeftPlane >= 0)
		{
			HitReactDirection = EHitReactDirection::Front;
		}
		else
		{
			HitReactDirection = EHitReactDirection::Back;
		}
	}
	else
	{
		if (FMath::Abs(DistanceToFrontBackPlane) <= FMath::Abs(DistanceToRightLeftPlane))
		{
			if (DistanceToRightLeftPlane >= 0)
			{
				HitReactDirection = EHitReactDirection::Front;
			}
			else
			{
				HitReactDirection = EHitReactDirection::Back;
			}
		}
		else
		{
			if (DistanceToFrontBackPlane >= 0)
			{
				HitReactDirection = EHitReactDirection::Right;
			}
			else
			{
				HitReactDirection = EHitReactDirection::Left;
			}
		}
	}

	bool IsStaggered = AttributeSet->GetPoise() - DamageAmount <= 0 ? true : false;

	if (AnimInstance)
	{
		AnimInstance->PlayHitReactMontage(HitReactDirection, DamageAmount, IsStaggered);
	}
	
	PlayHitSoundParticleEffect(InstigatorCharacter, DamageAmount);

}
```
---
#### APlayerCharacter  
- 입력
  - 같은 입력이라도 상태에 따라 다른 Ability가 동작할 수 있도록 AbilityTraversalComponent에서 Ability에 우선순위를 설정하여 Ability를 동작시킵니다.
> AbilityTraversalComponent.cpp 클릭 시 실행되는 TryNormalAttackTraversal
```  c++ 
bool UAbilityTraversalComponent::TryNormalAttackTraversal(UAbilitySystemComponent* ASC)
{
	for (TSubclassOf<UGameplayAbility> AbilityClass : TraversalNormalAttackAbilitiesOrdered)
	{
		if (ASC->TryActivateAbilityByClass(AbilityClass, true))
		{
			FGameplayAbilitySpec* Spec;

			Spec = ASC->FindAbilitySpecFromClass(AbilityClass);
			if (Spec && Spec->IsActive())
			{
				return true;
			}
		}
	}

	return false;
}

```
- 락온
  - 락온 Ability 발동 시, 플레이어와 락온한 타겟의 거리에 따라 카메라의 Roll 값이 변경되도록 Curve를 이용하였습니다
  - 이미지
> PlayerCharacter.cpp
``` c++
if (bIsLockOn && LockOnTarget)
	{
		if (LockOnCurve)
		{
			FRotator CurrentControllerRotation = GetController()->GetControlRotation();
			AEnemyCharacterBase* LockOnEnemy = Cast<AEnemyCharacterBase>(LockOnTarget);
			FVector TargetLocation = LockOnEnemy->GetLockOnLocation();

			float Distance = LockOnCurve->GetFloatValue(FVector::Dist(GetActorLocation(), LockOnTarget->GetActorLocation()));
			TargetLocation.Z -= Distance;

			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(FollowCamera->GetComponentLocation(), TargetLocation);
			FRotator InterpolateRotation = FMath::RInterpTo(CurrentControllerRotation, LookAtRotation, DeltaSeconds, 40.f);
			FRotator NewRotation = FRotator(InterpolateRotation.Pitch, InterpolateRotation.Yaw, CurrentControllerRotation.Roll);
			
			GetController()->SetControlRotation(NewRotation);
		}
	}
```
---
#### AEnemyCharacterBase
- 플레이어가 공격을 시도하면 Delegate를 통해 자동으로 가드를 시도합니다. 설정한 횟수만큼 가드에 성공하면 다음 공격은 패리 판정이 됩니다.
- 패리에 성공한 적은 BTService로 패리를 감지하며 바로 반격을 시도합니다.
> EnemyCharacterBase.cpp
``` c++
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
					// 일정 카운트 이상 가드에 성공 시, 패리
					if (bCanParry)
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

						bCanParry = false;

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
```
- MotionWarpingComponent를 이용하여 DeathBlow(인살)을 당할 시, 적의 위치가 몽타주 실행 중에 변경되도록 설정하였으며, DeathBlow의 종류에 따라 이동 위치를 다르게 설정하였습니다.
> EnemyCharacterBase.cpp
``` c++
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
```
- 적의 체력 게이지와 락온, 인살 대상이 되었을 때, 표시할 WidgetComponent를 구현하였습니다. 락온, 인살 WidgetComponent는 MeshSocket에 장착하여 WidgetComponent가 애니메이션 출력 중에도 따라가도록 설정하였습니다.
- 이미지 위젯 컴포넌트


---
#### AMinionCharacter
- AIPerception을 이용하여 플레이어가 시야에 들어왔을 때, 캐릭터 상단의 WidgetComponent의 게이지가 증가되도록 하였습니다. 완전히 차올랐을 시, 플레이어를 타겟으로 설정합니다. 
- USplineComponent로 MinionCharacter의 이동경로를 설정할 수 있도록 하였습니다. 게임 시작 시 경로를 반복하여 이동합니다. 
> MinionCharacter.cpp의 BeginPlay
``` c++
for (int i = 0; i < SplineComponent->GetNumberOfSplinePoints(); i++)
{
	PatrolPoints.Add(SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World));
}
SplineComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
```
---
#### ABossCharacter
- BossCharacter의 UBoxComponent에 플레이어가 Overlap되면 Boss의 타겟이 됩니다. 타겟이 되면 맵에 설정한 안개벽이 표시되며 HUD 상단에 Boss의 정보를 표시합니다.
> BossCharacter.cpp
``` c++
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
```

## LegIK
- 캐릭터의 스켈레톤에 발과 루트 소켓을 생성하였습니다. AnimInstance에서 매 프레임마다 이 소켓에서부터 LineTrace를 이용하여 양발의 위치와 각도, 루트의 위치를 계산하고 보간하여 이동합니다.
- 발 위치 이미지

## 발소리  
- PhyscalMaterial로 물체의 표면을 나타냅니다. USoundBase와 UNiagaraSystem을 이용하여 물체의 표면에 따라 다른 SFX와 VFX를 설정하였습니다.
- 피지컬 머티리얼 이미지
> SurfacePhysicalMaterial.h
``` c++
UCLASS()
class ACTIONADVENTURE_API USurfacePhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PhysicalMaterial)
	USoundBase* FootStepSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PhysicalMaterial)
	UNiagaraSystem* FootStepVFX;
};
```
- AA_CharacterBase는 UFootStepsComponent를 가지며, AnimNotify로 캐릭터의 발이 지면에 닿을 때, UFootStepsComponent에서 SFX와 VFX 그리고 소음을 발생시킵니다.
> FootStepsComponent.cpp
``` c++
void UFootStepsComponent::HandleFootStep(EFoot Foot)
{
	AAA_CharacterBase* OwnerCharacter = Cast<AAA_CharacterBase>(GetOwner());
	if (OwnerCharacter)
	{
		FVector SocketLocation = OwnerCharacter->GetMesh()->GetSocketLocation(Foot == EFoot::Left ? LeftFootSocketName : RightFootSocketName);

		FVector Start(SocketLocation.X, SocketLocation.Y, SocketLocation.Z + 20.f);
		FVector End(SocketLocation.X, SocketLocation.Y, SocketLocation.Z - 50.f);

		FHitResult HitResult = FHitResult(ForceInit);
		FCollisionQueryParams Params(TEXT("FootStepTrace"), false, OwnerCharacter);
		Params.bReturnPhysicalMaterial = true;
		bool FootStepTraceHited = OwnerCharacter->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_WorldStatic, Params);
		if (FootStepTraceHited)
		{
			if (HitResult.PhysMaterial.Get())
			{
				USurfacePhysicalMaterial* PhysicalMaterial = Cast<USurfacePhysicalMaterial>(HitResult.PhysMaterial.Get());
				if (PhysicalMaterial)
				{
					UGameplayStatics::PlaySoundAtLocation(this, PhysicalMaterial->FootStepSound, SocketLocation, FootStepVolumeMultiplier);
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PhysicalMaterial->FootStepVFX, SocketLocation);
				}
			}

			UAISense_Hearing::ReportNoiseEvent(
				OwnerCharacter->GetWorld(),
				SocketLocation,
				0.2f,
				OwnerCharacter,
				0.f,
				FName("Noise")
			);
		}
	}
}
```

## 갈고리  
#### 갈고리 Ability의 메커니즘
- 포인트의 일정 범위에 들어오면 포인트를 인식합니다. 
- 인식된 포인트는 카메라와의 방향과 카메라 정면 방향의 내적값이 계산됩니다. 
- 인식된 포인트 중 최적의 내적값을 가진 포인트가 FocusedPoint로 선택됩니다.
- FocusedPoint가 화면의 화면에 들어온다면 WidgetComponent를 출력합니다.
- FocusedPoint가 뷰포트의 일정 범위를 벗어난다면 화살표가 있는 WidgetComponent로 교체되어 FocusedPoint가 있는 방향을 표시해줍니다. 
- FocusedPoint와의 거리에 따라 WidgetCompnent의 Material을 변경하며 상호작용 가능한 거리가 되었을 때, 색상이 변합니다. 
- Ability 실행 시, 플레이어가 공중에 있는지, 포인트와의 상하 위치에 따라 출력된느 몽타주가 달라집니다. 
- 몽타주 실행 중, AnimNotifyState로 포인트의 위치로 플레이어가 이동하며, UCableComponent가 포인트까지 늘어납니다.
---
#### UGrapplingHookComponent
- Player는 UGrapplingHookComponent를 가지고 있으며, 갈고리와의 상호작용을 담당합니다. 
- 포인터의 일정 범위에 들어가면 TArray 컨테이너인 HookablePoints에 들어갑니다. 
- HookablePoints에 들어간 포인트는 틱마다 카메라의 방향과 내적값이 계산되어 TMap 컨테이너인 PointsFacingRate에 내적값과 들어갑니다.
> GrapplingHookComponent.cpp
``` c++
// Tick에서 호출
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
```
- PointsFacingRate 중 최적의 값을 가진 포인트를 FocusedPoint로 설정합니다. 
> GrapplingHookComponent.cpp
``` c++
// Tick에서 호출
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
```
- GrapplingHookAbility를 실행하면 공중 유무와 FocuedPoint와 상하 위치에 따라 출력할 몽타주를 달리하여 출력합니다. 몽타주는 UDataTable의 형태로 저장됩니다.
> GGrapplingHookComponent.cpp
``` c++
// GrapplingHookAbility에서 호출
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
```
---
#### AHookablePoint
- HookablePoint는 BoxComponent를 가지며, Overlap 시, GrapplingHookComponent의 HookablePoints에 들어갑니다. 
- Ability 실행 시, CableComponent가 도달할 위치와 플레이어가 이동할 에디터 상에서 편리하게 설정할 수 있도록 BillboradComponent를 가집니다.
- 포인터가 FocusedPoint가 되었을 시, 표시할 WidgetComponent인 HookablePointWidget을 가집니다.
- 에디터에서의 이미지

---
#### UHookablePointWidget
- 표시할 UserWidget인 HookablePointTracker을 가집니다.
- 틱마다 뷰포트의 일정 범위를 벗어났는지 판별할 ConstrainScreenBounds를 실행합니다.
- 일정 범위를 벗어났다면 화살표가 있는 Widget을 표시합니다.
> HookablePointWidget.cpp
``` c++
// 틱에서 호출
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
```
---
#### UHookablePointTracker
- UserWidget으로 플레이어와의 거리가 가까워질 수록 WIdget의 Material의 파라미터를 변경하여 원이 점차 채워지며, Ability를 실행할 수 있을 만큼 가까워 졌을 경우 색상을 변경합니다.
> HookablePointTracker.cpp
``` c++
void UHookablePointTracker::UpdatetTrackerPointMaterial(float CharacterPointDistance)
{
	float MaterialScaleRate = 0.f;

	MaterialScaleRate = -1.f * (CharacterPointDistance / StartUpdateDistance) + 1.f;
	MaterialScaleRate *= ScaleRateMultiplier;
	
	TrackerPointMat->SetScalarParameterValue("Radius", FMath::Clamp(MaterialScaleRate, MinScaleValue, MaxScaleValue));

	// 캐릭터가 갈고리를 걸 수 있는 거리라면 색을 변환
	if (MaterialScaleRate >= MaxScaleValue)
	{
		TrackerPointMat->SetVectorParameterValue("Color", MaxScaleColor);
		IsTrackerFull = true;
	}
	else
	{
		TrackerPointMat->SetVectorParameterValue("Color", NormalScaleColor);
		IsTrackerFull = false;
	}
}
```

## 상호작용  
#### 상호작용의 메커니즘
- 갈고리 Ability와 유사하게 작동합니다.
- 플레이어와 일정 범위안에 들어오면 Actor를 인식합니다.
- 인식된 Actor는 WidgetComponent를 출력합니다.
- 인식된 Actor는 플레이어의 정면 방향과의 내적값이 계산됩니다. 
- 인식된 Actor 중 최적의 내적값을 가진 Actor가 FocusedActor로 선택됩니다.
- FocusedActor는 플레이어가 알아보기 쉽도록 변경된 WidgetComponent를 출력합니다.
- Actor가 뷰포트의 일정 범위를 벗어나거나, 벽에 가려져서 카메라에 잡히지 않는다면 WidgetComponent가 출력되지 않습니다.

---
#### UInteractionComponent
- Player는 InteractionComponent를 가지고 있으며, 상호작용을 담당합니다.
- Actor의 일정 범위에 들어가면 TArray 컨테이너인 InteractionActors에 들어갑니다.
- InteractionActors에 들어간 포인트는 틱마다 플레이어의 정면 방향과 내적값이 계산되어 TMap 컨테이너인 ActorsFacingRate에 내적값과 들어갑니다.
> CInteractionComponent.cpp
``` c++
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
```
- ActorFacingRate 중 최적의 내적값을 가진 Actor를 FocusedActor로 지정합니다.
> InteractionComponent.cpp
``` c++
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
```
---
#### AInteractionActorBase
- InteractionInterface를 상속하며 상호작용가능한 액터의 베이스입니다.
- InteractionActorBase가 플레이어의 일정 범위 안에 들어오면, InteractionComponent의 InteractionActors에 들어갑니다.
- InteractionActors에 들어가면 WidgetComponent인 OnScreenBoundsWidgetComp가 출력됩니다. 
- OnScreenBoundsWidgetComp는 장애물에 가려지거나 뷰포트의 일정 범위를 벗어나면 출력되지 않습니다.
> InteractionActorBase.cpp
``` c++
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
```
- FocusedActor가 되었을 때, 표시할 WidgetComponent인 StartFocusedWidget을 가지고 있습니다. Widget에서 출력할 텍스트도 지정할 수 있습니다.
- 포커스되었을 때 이미지
- InteractionActorBase는 InteractionInterface를 상속받습니다. FoucedActor와 상호작용 시, 이 인터페이스의 OnInteract가 실행됩니다.
> InteractionInterface.h
``` c++
UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONADVENTURE_API IInteractionInterface
{
	GENERATED_BODY()

public:
	virtual void OnScreenBounds() = 0;
	virtual void OnOutScreenBounds() = 0;
	virtual void StartFocus() = 0;
	virtual void EndFocus() = 0;
	virtual bool OnInteract(AActor* Caller) = 0;
};
```
---
#### AItemPickupActor
- 상호작용 시, 아이템을 획득할 수 있는 Actor로 AInteractionActorBase를 상속합니다.
- 획득할 아이템 데이터인 UAA_Item과 수량을 설정할 수 있습니다. 
> ItemPickupActor.cpp
``` c++
bool AItemPickupActor::OnInteract(AActor* Caller)
{
	if (!Super::OnInteract(Caller))
	{
		return false;
	}

	SavePickedUp();

	GiveItem();

	return true;
}

void AItemPickupActor::GiveItem()
{
	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController)
	{
		PlayerController->ShowItemPickupUI(ItemType);
		PlayerController->AddInventoryItem(ItemType, ItemCount, false);

		Destroy();
	}
}
```
----
#### ACheckPoint
- 플레이어의 휴식과 CheckPoint간의 이동을 할 수 있는 Actor로 AInteractionActorBase를 상속합니다. 
- 상호작용한 CheckPoint는 GameInstance를 상속한 AA_ItemInstance에 TArray의 형태로 저장됩니다. 
- 플레이어가 죽었을 시, 가장 최근에 상호작용한 CheckPoint로 이동합니다. 
- CheckPoint에서 휴식하면 GameInstance내의 Delegate가 Broadcast됩니다.
- 휴식 시, 캐릭터의 경우 Attribute를 모두 회복하는 GameplayEffect를 적용하고 AbilitySystemComponent와 Controller를 초기화시킵니다. 

## 전투
#### UAA_GameplayAbility
프로젝트의 GameplayAbility의 베이스 클래스입니다. 
- Ability의 실행 직후 적용할 GameplayEffect와 종료 시 함께 삭제될 GameplayEffect 그리고 종료 시 적용할 GameplayEffect를 설정할 수 있습니다.
- 이미지
> AA_GameplayAbility.h
``` c++
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TArray<TSubclassOf<UGameplayEffect>> OngoingEffectsToRemoveOnEnd;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TArray<TSubclassOf<UGameplayEffect>> OngoingEffectsToJustApplyOnStart;

	TArray<FActiveGameplayEffectHandle> RemoveOnEndEffectHandles;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TArray<TSubclassOf<UGameplayEffect>> EffectsToJustApplyOnEnd;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TMap<FGameplayTag, FGameplayEffectContainer> EffectMap;
```
- GameplayEvent를 받았을 경우 받은 Event의 Tag에 해당하는 GameplayEffect를 적용할 수 있습니다.
> AA_GameplayAbility.cpp
``` c++
TArray<FGameplayEffectSpecHandle> UAA_GameplayAbility::MakeEffectSpec(FGameplayTag Tag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	TArray<FGameplayEffectSpecHandle> ReturnSpec;
	if (UAbilitySystemComponent* AbilityComponent = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayEffectContainer* GameplayEffectContainer = EffectMap.Find(Tag);
		if (GameplayEffectContainer)
		{
			TArray<TSubclassOf<UGameplayEffect>> EffectClasses = GameplayEffectContainer->GameplayEffectClasses;
			for (TSubclassOf<UGameplayEffect>& EffectClass : EffectClasses)
			{
				ReturnSpec.Add(MakeOutgoingGameplayEffectSpec(EffectClass, GetAbilityLevel()));
			}
		}
	}

	return ReturnSpec;
}

TArray<FActiveGameplayEffectHandle> UAA_GameplayAbility::ApplyEffectSpec(TArray<FGameplayEffectSpecHandle>& Spec, const FGameplayEventData& EventData)
{
	TArray<FActiveGameplayEffectHandle> AllEffects; 
	for (const FGameplayEffectSpecHandle& SpecHandle : Spec)
	{
		AllEffects.Append(K2_ApplyGameplayEffectSpecToTarget(SpecHandle, EventData.TargetData));
	}

	return AllEffects;
}

TArray<FActiveGameplayEffectHandle> UAA_GameplayAbility::ApplyEffect(FGameplayTag Tag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	TArray<FGameplayEffectSpecHandle> Spec = MakeEffectSpec(Tag, EventData, GetAbilityLevel());
	
	return ApplyEffectSpec(Spec, EventData);
}
```
---
#### Melee Attack Ability
공격하는 캐릭터를 A, 공격받는 캐릭터를 B라고 하겠습니다.
- A가 Melee Ability를 실행하면 커스텀 AbilityTask인 AT_PlayerMontageAndWaitForEvent에 의해 지정해둔 몽타주가 실행되며 동시에 GameplayEvent를 받을 수 있습니다.
> GA_Melee.cpp
``` c++
void UGA_Melee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayTagContainer GameplayTagContainer = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("Event.Montage")));
	MontageTask = UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, GameplayTagContainer);
	MontageTask->OnBlendOut.AddDynamic(this, &UGA_Melee::OnCompleted);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_Melee::OnCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_Melee::OnCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_Melee::OnCancelled);
	MontageTask->EventReceived.AddDynamic(this, &UGA_Melee::EventReceived);
	MontageTask->ReadyForActivation();
}
```
- 몽타주 이미지 
- AnimNotifyState_WeaponAttack으로 무기의 피격시간과 AttackEventTag를 설정합니다.
- A의 무기가 B에 Overlap되었을 시, 미리 지정한 AttackEventTag와 B를 타겟으로 설정한 GameplayEvent를 A에게 전송합니다. 
> WeaponActor.cpp
``` c++
void AWeaponActor::CapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((GetOwner() != OtherActor) && bIsAttacking && OverlapedActors.Find(OtherActor))
	{
		AAA_CharacterBase* OwnerCharacter = Cast<AAA_CharacterBase>(GetOwner());
		AAA_CharacterBase* TargetCharacter = Cast<AAA_CharacterBase>(OtherActor);
		if (OwnerCharacter && TargetCharacter)
		{
			if (!OwnerCharacter->IsEnemy(TargetCharacter->GetTeamNumber()))
			{
				return;
			}
		}
		if (TargetCharacter->IsValidLowLevel())
		{
			if (TargetCharacter->IsInvincible())
			{
				return;
			}
		}

		OverlapedActors.Add(TargetCharacter);

		FGameplayEventData EventPayload;
		EventPayload.Instigator = OwnerCharacter;
		EventPayload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(TargetCharacter);
		EventPayload.Target = TargetCharacter;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerCharacter, AttackEventTag, EventPayload);
		
	}
}
```
- GameplayEvent를 전송받은 A는 AttackEventTag에 따라 타겟인 B에게 GameplayEffect를 적용하여 B의 Attribute를 변화시킵니다.

---
#### 콤보 공격
- 플레이어의 경우 일반 공격 중에 다시 공격을 시도하면 콤보 공격을 할 수 있습니다.
- 몽타주 이미지
- AnimNotifyState_JumpSection으로 해당 구간에서 공격 시도 시, 몽타주의 Section을 넘어가서 실행하는 것으로 구현하였습니다.
> PlayerCharacter.cpp의 JumpSectionForCombo
``` c++
if (AnimInstance)
{
	UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
	FName CurrentSection = AnimInstance->Montage_GetCurrentSection(CurrentMontage);
	FName NextSection = JumpSectionNotify->JumpSection;
	AnimInstance->Montage_JumpToSection(NextSection, CurrentMontage);

	SetEnableComboPeriod(false);
}
```
---
#### DeathBlow Ability
DeathBlow Ability(인살)는 적이 가지고 있는 목숨(DeathCount) Attribute를 1차감 시키는 Ability로 총 4가지 종류가 있습니다.

- DeathBlow Ability는 적의 Stamina가 0이 되어 그로기 상태가 되었을 때, 실행할 수 있습니다.
- 그로기 GameplayTag를 가지고 있는지가 그로기 판별 유무입니다.
- Ability를 시도하면 SweepMultiByChannel을 이용하여 타겟 캐릭터를 찾습니다. 
- 타겟 캐릭터는 피격 몽타주를 실행하며 실행 중 MotionWarping에 의해 알맞은 위치로 이동하며 AnimNotify를 이용하여 특정 시점에 GameplayEffect를 적용하여, DeathCount Attribute를 감소시킵니다.
> GA_DeathBlow.cpp
``` c++
void UGA_DeathBlow::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayTag MontageTag = FGameplayTag::RequestGameplayTag(FName("Event.Montage.DeathBlow.Default"));
	UAnimMontage* MontageToPlay = *OwnerCharacter->GetCharacterData().CharacterAnimDataAsset->CharacterAnimationData.DeathBlowMontageMap.Find(MontageTag);

	FGameplayTagContainer GameplayTagContainer = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.DeathBlow.Default")));
	MontageTask = UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, GameplayTagContainer);
	MontageTask->OnBlendOut.AddDynamic(this, &UGA_DeathBlow::OnCompleted);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_DeathBlow::OnCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_DeathBlow::OnCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_DeathBlow::OnCancelled);
	MontageTask->EventReceived.AddDynamic(this, &UGA_DeathBlow::EventReceived);
	MontageTask->ReadyForActivation();

	if (TargetCharacter.Get())
	{
    	// MotionWarping과 피격 몽타주 실행
		TargetCharacter->HandleDeathBlow(MontageTag, OwnerCharacter.Get());
	}
}
```
---
#### BackstabDeathBlow Ability
- 적의 배후에서의 DeathBlow Ability입니다. 
- 플레이어는 매 틱마다 일정 범위 내에 있는 적들 중 가장 가까이 있는 적이 BackstablDeathBlow가 가능한지 검사합니다.
- 가능 유무는 플레이어의 위치와의 내적값으로 전후를 판별하며, 현재 타겟으로 삼고 있는 캐릭터가 없을 경우 가능합니다.
> EnemyCharacterBase.cpp
``` c++
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
```

---
#### PlungeDeathBlow Ability
- 공중에서의 DeathBlow Ability입니다. 
- 플레이어는 매 틱마다 락온한 타겟이 PlungeDeathBlow가 가능한지 검사합니다.
- 가능 유무는 플레이어와의 수직 거리가 일정 거리 이상, 수평 거리가 일정 거리 미만이며, 사이에 장애물이 없을 경우 가능합니다. 
> PlayerCharacter.cpp
``` c++
// 틱마다 호출
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
```
- Ability를 시행하면 몽타주 출력과 동시에 AblityTask_PlungeToTarget이 실행되며 타겟에게 이동합니다.
> AT_PlungeToTarget.cpp
``` c++
void UAT_PlungeToTarget::TickTask(float DeltaTime)
{
	if (bIsFinished)
	{
		return;
	}

	Super::TickTask(DeltaTime);

	
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetAvatarActor());
	if (OwnerCharacter)
	{
		FVector CharacterLocation = OwnerCharacter->GetActorLocation();
		FVector TargetLocation = Target->GetActorLocation();

		FRotator EndRot = UKismetMathLibrary::FindLookAtRotation(CharacterLocation, TargetLocation);
		FVector EndRotVector = EndRot.Vector();
		EndRotVector.Z *= 0.5f;

		// 타겟에게 이동
		TargetLocation.Z = CharacterLocation.Z - 300.f;
		FVector Direction = (TargetLocation - CharacterLocation).GetSafeNormal();
		OwnerCharacter->SetActorLocation(OwnerCharacter->GetActorLocation() + 30.f * EndRotVector);

	}

	// 일정 범위에 들어왔다면 Task 종료
	FVector TargetLocation2D = FVector(Target->GetActorLocation().X, Target->GetActorLocation().Y, 0.f);
	FVector MyCharacterLocation2D = FVector(OwnerCharacter->GetActorLocation().X, OwnerCharacter->GetActorLocation().Y, 0.f);
	if (FVector::Dist(TargetLocation2D, MyCharacterLocation2D) < 30.f)
	{
		bIsFinished = true;
		EndTask();
	}
}
```
- 타겟이 플레이어와 일정 범위 안에 들어왔다면 피격 몽타주를 출력합니다.
---
#### FinisherDeathBlow Ability
- 보스의 DeathCount가 0이 되면 실행가능한 DeathBlow입니다. 
- 보스의 DeathCount가 0이 되면 그로기 상태가 되며 플레이어는 Ability 실행 가능 상태가 됩니다. SetGlobalTimeDilation을 사용하여 시간이 느려지게 설정하였습니다. 
> BossCharacter.cpp
``` c++
void ABossCharacter::OnDeathCountDepleted()
{
	PlayFinishedGroggyMontage();

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerCharacter->IsValidLowLevel())
	{
		PlayerCharacter->SetFinishDeathBlowState(this);
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
```
- 만약 몽타주가 끝날 때까지 Ability를 실행하지 않았다면 보스는 Attribute의 일부를 회복한 상태가 되며, SetGlobalTimeDilation을 다시 설정하여 기존의 상태로 돌아옵니다.
> BossCharacter.cpp
``` c++
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
```
---
#### Guard Ability
- Guard Ability는 실행 시, 공격을 막을 수 있으며 캐릭터의 이동속도가 느려집니다.
- 현재 GuardAbility의 실행 유무와 공격자와의 내적을 통해 가드와 패리 판정을 합니다.
> AAA_CharacterBase.cpp 가드 판정
```c++
bool AAA_CharacterBase::IsGuarding() const
{
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer GameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("Ability.Guard")));
		TArray<UAA_GameplayAbility*> ActiveAbilities;
		AbilitySystemComponent->GetActiveAbilitiesWithTags(GameplayTagContainer, ActiveAbilities);

		return ActiveAbilities.Num() > 0;
	}

	return false;
}

bool AAA_CharacterBase::GuardCheck(const AActor* OtherActor) const
{
	if (!IsGuarding())
	{
		return false;
	}

	float DistanceToFrontBackPlane = FVector::PointPlaneDist(OtherActor->GetActorLocation(), GetActorLocation(), GetActorRightVector());
	float DistanceToRightLeftPlane = FVector::PointPlaneDist(OtherActor->GetActorLocation(), GetActorLocation(), GetActorForwardVector());
	if (FMath::Abs(DistanceToFrontBackPlane) <= FMath::Abs(DistanceToRightLeftPlane))
	{
		if (DistanceToRightLeftPlane >= 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}
```
- 플레이어의 Guard Ability는 추가적으로 패리가 가능합니다. Ability 실행 직후, GameplayEffect를 이용하여 일정 시간 동안 패리 가능 상태가 됩니다.
- Guard를 연타하면 패리 GameplayEffect의 AbilityLevel을 증가시켜 패리의 지속 시간이 짧아지도록 만들었습니다.
> GA_PlayerGuard.cpp
``` c++
void UGA_PlayerGuard::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	float DeflectLevel = 0.f;
	UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get();
	if (AbilityComponent)
	{
		// 방어를 연타하였을 때, 패링 시간을 줄인다. 
		APlayerCharacter* Character = CastChecked<APlayerCharacter>(ActorInfo->AvatarActor.Get());
		if (Character)
		{
			if (Character->GetDeflectTimerPending())
			{
				Character->SetDeflectLevel(Character->GetDeflectLevel() + 1.f);
				DeflectLevel = Character->GetDeflectLevel();
			}
			DeflectLevel = DeflectLevel > 5.f ? 5.f : DeflectLevel;
			Character->SetDeflectTimer();
		}

		FGameplayEffectContextHandle EffectContext = AbilityComponent->MakeEffectContext();
		if (DeflectEffect.Get())
		{
			FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(DeflectEffect, DeflectLevel, EffectContext);
			FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			if (!ActiveGEHandle.WasSuccessfullyApplied())
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to apply startup effect! %s"), *GetNameSafe(DeflectEffect));
			}
		}
	}
}
```
- 패리에 성공하면 공격 시전자에게 GameplayEffect를 적용하여 일정 수준의 피해를 입힙니다.
> PlayerCharacter.cpp
``` c++
void APlayerCharacter::HandlePoiseDamage(float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags, AAA_CharacterBase* InstigatorCharacter, AActor* DamageCauser)
{
	Super::HandlePoiseDamage(DamageAmount, HitInfo, DamageTags, InstigatorCharacter, DamageCauser);

	// 패리 성공 시, 적에게 스태미나 데미지
	if (DeflectCheck(InstigatorCharacter))
	{
		FGameplayEffectContextHandle ContextHandle;
		ContextHandle.AddInstigator(this, Cast<AActor>(Weapon));
		ContextHandle.AddSourceObject(this);
		InstigatorCharacter->ApplyGameplayEffectToSelf(DeflectEffect, ContextHandle);
	}
}
```
---
#### FireProjectile Ability
- 투사체를 발사하는 Ability 입니다. 영상에서는 수리검에 해당 Ability를 사용했습니다. 
- Ability를 실행하면 몽타주를 출력하며, AnimNotify로 특정 순간에 GameplayEvent를 전달받습니다.
- 전달받은 순간 투사체를 생성합니다.
- 몽타주 이미지
> GA_FireProjectile.cpp
``` c++
void UGA_FireProjectile::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::ApplyEffect(EventTag, EventData);

	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.SpawnProjectile")))
	{
		APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwningActorFromActorInfo());
		if (!IsValid(OwnerCharacter))
		{
			return;
		}

		FVector Start = OwnerCharacter->GetMesh()->GetSocketLocation(FName("MuzzleSocket"));
		FVector End = OwnerCharacter->GetActorLocation() +
			OwnerCharacter->GetActorForwardVector() * Range;
		if (OwnerCharacter->GetIsLockOn())
		{
			End = OwnerCharacter->GetLockOnTarget()->GetActorLocation();
		}
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, End);

		FTransform MuzzleTransform = OwnerCharacter->GetMesh()->GetSocketTransform(FName("MuzzleSocket"));
		MuzzleTransform.SetRotation(Rotation.Quaternion());
		MuzzleTransform.SetScale3D(FVector(1.f));

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AAA_Projectile* Projectile = GetWorld()->SpawnActorDeferred<AAA_Projectile>(ProjectileClass, MuzzleTransform,
			GetOwningActorFromActorInfo(), OwnerCharacter, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		Projectile->Range = Range;
		Projectile->FinishSpawning(MuzzleTransform);
	}	
}
```
---
#### CombatArt Ability
- 무기의 공격 범위를 늘려서 공격하거나 돌진 공격을 할 수 있는 Ability입니다. 
- 무기의 CapsuleComponent를 따로 두어 CombatAbility는 해당 CapsuleComponent를 사용하는 것으로 구현하였습니다.
- 무기 이미지 에디터 에서
- 돌진 공격의 경우 최대 거리를 설정하여 타겟과의 거리가 최대 거리 이하라면 타겟 바로 앞까지 이동하며, 이상이라면 최대 거리까지만 이동하도록 설정하였습니다.
> GA_CombatArtBase.cpp
``` c++
void UGA_CombatArtBase::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::ApplyEffect(EventTag, EventData);

	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.Shared.SetRushLocation")))
	{
		if (bNeedMotionWarping)
		{
			AAA_CharacterBase* OwnerCharacter = Cast<AAA_CharacterBase>(GetOwningActorFromActorInfo());
			if (OwnerCharacter->GetTargetCharacter() != nullptr)
			{
				FVector TargetLocation = OwnerCharacter->GetTargetCharacter()->GetActorLocation();
				FVector OwnerLocation = OwnerCharacter->GetActorLocation();
				if (FVector::Dist(TargetLocation, OwnerLocation) > RushDistance)
				{
					OwnerCharacter->GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocation(
						TEXT("Rush"), OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * RushDistance);
				}
				else
				{
					OwnerCharacter->GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocation(
						TEXT("Rush"), TargetLocation - OwnerCharacter->GetActorForwardVector() * 150.f);
				}
			}
			else
			{
				OwnerCharacter->GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocation(
					TEXT("Rush"), OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * RushDistance);
			}
		}
	}
}
```

## 데미지 처리
데미지는 Damage Attribute로 관리되며, Damage, StaminaDamage, PoiseDamage 총 세 가지입니다. 각 Attribute는 체력, 스태미나, 강인도 Attribute에 관여됩니다. 또한, 방어력이나 공격에 관련된 버프 Attribute도 데미지 처리에 관여합니다. 버프 Attribute는 AttackPowner, HealthDefensePower, StaminaDefensePower 총 세 가지 입니다.   

---
#### AA_DamageExecutionCalculation
- 데미지를 주는 GameplayEffect의 Execution을 AA_DamageExecutionCalculation으로 설정하여 데미지 계산을 수행하도록 하였습니다.
- gameplayeffect 이미지
- AA_DamageExecutionCalculation은 데미지에 영향을 주는 Attribute를 Target과 Source에서부터 캡쳐하여 데미지 계산을 수행합니다.
> AA_DamageExecutionCalculation.cpp
``` c++
void UAA_DamageExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float HealthDefensePower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().HealthDefensePowerDef, EvaluationParameters, HealthDefensePower);
	if (HealthDefensePower == 0.0f)
	{
		HealthDefensePower = 1.0f;
	}
	float StaminaDefensePower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().StaminaDefensePowerDef, EvaluationParameters, StaminaDefensePower);
	if (StaminaDefensePower == 0.0f)
	{
		StaminaDefensePower = 1.0f;
	}

	float AttackPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackPowerDef, EvaluationParameters, AttackPower);

	float Damage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluationParameters, Damage);
	Damage += FMath::Max<float>(Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), false, -1.0f), 0.0f);

	float StaminaDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().StaminaDamageDef, EvaluationParameters, StaminaDamage);

	float PoiseDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PoiseDamageDef, EvaluationParameters, PoiseDamage);

	if (PoiseDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().PoiseDamageProperty, EGameplayModOp::Additive, PoiseDamage));
	}

	float DamageDone = Damage * AttackPower / HealthDefensePower;
	if (DamageDone > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, DamageDone));
	}

	float StaminaDamageDone = StaminaDamage * AttackPower / StaminaDefensePower;
	if (StaminaDamageDone > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().StaminaDamageProperty, EGameplayModOp::Additive, StaminaDamageDone));
	}
}
```
---
#### 강인도
- 캐릭터의 강인도 Attribute가 공격의 PoiseDamage보다 낮다면 경직이 발생합니다.
- 강인도와 PoiseDamage를 조절하여 강공격에는 큰 경직을 유발하거나 공격 중에는 경직이 발생하지 않도록 조절할 수 있습니다.
> AA_AttributeSetBase.cpp
``` c++
if (Data.EvaluatedData.Attribute == GetPoiseDamageAttribute())
	{
		const float LocalPoiseDamageDone = GetPoiseDamage();
		SetPoiseDamage(0.f);

		// 타겟의 강인도가 공격의 강인도보다 낮을 경우 경직을 유발한다.
		bool WasAlive = true;
		if (TargetCharacter)
		{
			WasAlive = TargetCharacter->IsAlive();
		}

		if (TargetCharacter && WasAlive && SourceCharacter && GetHealth() > 0.f)
		{
			TargetCharacter->HandlePoiseDamage(LocalPoiseDamageDone, *Context.GetHitResult(), SourceTags, SourceCharacter, SourceActor);
		}
	}
```


## AI  
#### BTTask_MoveSide
- 무작위 시간동안 적이 좌우로 이동하는 BTTask입니다. 
- AddMovementInput과 움직이는 시간을 무작위로 산출하여 구현하였습니다. 
- Node에서 직접적으로 움직이는 시간과 편차를 설정할 수 있도록 하였습니다.
> BTTask_MoveSide.cpp
``` c++
void UBTTask_MoveSide::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AccumulatedDeltaTime += DeltaSeconds;
	AccumulatedMoveDirectionDeltaTime += DeltaSeconds;

	if (AccumulatedDeltaTime >= RemainingWaitTime)
	{
		AccumulatedDeltaTime = 0.0f;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (AccumulatedMoveDirectionDeltaTime >= RemainigChangeDirectionTime)
	{
		AccumulatedMoveDirectionDeltaTime = 0.0f;
		RemainigChangeDirectionTime = FMath::FRandRange(1.0f, 2.0f);

		MoveDirection = !MoveDirection;
	}

	AEnemyCharacterBase* OwnerCharacter = Cast<AEnemyCharacterBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (OwnerCharacter)
	{
		if (MoveDirection)
		{
			OwnerCharacter->AddMovementInput(OwnerCharacter->GetActorRightVector());
		}
		else
		{
			OwnerCharacter->AddMovementInput(-(OwnerCharacter->GetActorRightVector()));
		}
	}
	ensure(GetSpecialNodeMemory<FBTTaskMemory>(NodeMemory)->NextTickRemainingTime <= 0.f);
```
- 노드 이미지

---
#### BTService_DetectDeflect
- 적이 패리를 성공하면 즉시 반격을 하도록 설정하였습니다. BTService_DetectDeflect에서 패리를 감지합니다.
- 노드 이미지

---
#### BTTask_TurnTowardNoise
- AIController에서 AIPerception을 이용하여 Noise를 탐지합니다. 탐지한 Noise의 위치는 Blackborad Key로 관리됩니다.
> AA_AIController.cpp
``` c++
void AAA_AIController::OnTargetUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// Noise를 탐지했을 경우
	if (Stimulus.Tag == FName("Noise"))
	{
		if (Actor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
		{
			GetBlackboardComponent()->SetValueAsBool(AAA_AIController::InvestigatingKey, true);
			GetBlackboardComponent()->SetValueAsVector(AAA_AIController::NoiseLocationKey, Stimulus.StimulusLocation);
		}
	}
    // 손가락 피리를 들었다면 Noise의 위치로 이동
	else if (Stimulus.Tag == FName("FingerWhistle"))
	{
		GetBlackboardComponent()->SetValueAsBool(AAA_AIController::InvestigatingByFingerWhistleKey, true);
		GetBlackboardComponent()->SetValueAsVector(AAA_AIController::NoiseLocationKey, Stimulus.StimulusLocation);
	}
	else
	{
		if (Actor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
		{
			OnPerceptionDetect.Broadcast(Stimulus.WasSuccessfullySensed() && Actor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0), 0.0f);
		}
	}
	
}
```
- Noise를 탐지했다면 Noise의 방향으로 회전합니다. 
> BTTask_TurnTowardNoise.cpp
``` c++
EBTNodeResult::Type UBTTask_TurnTowardNoise::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAA_AIController* const OwnerAI = Cast<AAA_AIController>(OwnerComp.GetAIOwner());
	if (!OwnerAI)
	{
		return EBTNodeResult::Failed;
	}

	FVector NoiseLocation = OwnerAI->GetBlackboardComponent()->GetValueAsVector(AAA_AIController::NoiseLocationKey);

	FRotator CurrentControllerRotation = OwnerAI->GetControlRotation();
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(OwnerComp.GetOwner()->GetActorLocation(), NoiseLocation);
	FRotator NewRotation = FRotator(CurrentControllerRotation.Pitch, LookAtRotation.Yaw, CurrentControllerRotation.Roll);

	OwnerAI->GetPawn()->SetActorRotation(FMath::RInterpTo(CurrentControllerRotation,
		NewRotation, GetWorld()->GetDeltaSeconds(), 360.f));

	return EBTNodeResult::Succeeded;
}
```
---
#### BTTask_UseAbility
- 부여된 Ability는 부여된 GameplayTag로 실행할 수 있습니다. 
- 하나의 노드에 GameplayTag를 추가하여 임의의 Ability를 실행할 수 있도록 하였습니다.
- 실행중인 Ability가 종료되기 전에는 Task를 진행 상태로 두어 오류를 방지하였습니다.
> BTTask_UseAbility.cpp
``` c++
EBTNodeResult::Type UBTTask_UseAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AAA_CharacterBase* OwnerCharacter = Cast<AAA_CharacterBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (IsValid(OwnerCharacter))
	{
		TArray<FGameplayTag> GameplayTags;
		GameplayAbilityTags.GetGameplayTagArray(GameplayTags);
		if (GameplayTags.Num() > 0)
		{
			int32 TagIndex = FMath::RandRange(1, GameplayTags.Num()) - 1;
			FGameplayTagContainer ActiveTagContainer(GameplayTags[TagIndex]);

			if (OwnerCharacter->GetAbilitySystemComponent()->TryActivateAbilitiesByTag(ActiveTagContainer, true))
			{
				TArray<UAA_GameplayAbility*> ActiveAbilities;
				OwnerCharacter->GetActiveAbilitiesWithTags(ActiveTagContainer, ActiveAbilities);
				if (ActiveAbilities.Num() > 0)
				{
					ActiveAbilities[0]->OnGameplayAbilityEnded.AddUFunction(this, FName("OnAbilityEnded"));
					bIsActivating = true;
					Result = EBTNodeResult::InProgress;
				}
			}
		}
		
	}

	return Result;
}

void UBTTask_UseAbility::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bIsActivating)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTTask_UseAbility::OnAbilityEnded(UGameplayAbility* GameplayAbility)
{
	bIsActivating = false;
}
```

## 인벤토리
인벤토리 UI 이미지

#### UAA_Item
- AA_Item은 PrimaryDataAsset을 상속합니다. 
- 아이템의 실제 데이터로 아이템 타입과 이름, 설명, 장착 했을 때 부여되는 Ability 등을 가집니다. 
- 아이템 이미지

---
#### FAA_ItemData
- AA_Item의 개수를 들고있는 구조체입니다. 
- 아이템을 획득하면 FAA_ItemData가 변경됩니다.

---
#### FAA_ItemSlot
- 인벤토리의 장착 슬롯을 나타내는 구조체입니다. 
- 슬롯의 아이템 타입과 슬롯의 번호를 가지고 있습니다. 

---
#### 인벤토리
- 인벤토리는 AA_PlayerController에서 관리됩니다. 
- 현재 인벤토리 내의 아이템은 TMap<UAA_Item*, FAA_ItemData>의 형태로 존재합니다. 
- 장착중인 아이템은 TMap<FAA_ItempSlot, UAA_Item*>의 형태로 존재합니다. 
- 아이템을 획득하면 획득한 아이템에 해당하는 FAA_ItemData의 수량이 증가합니다. 
> AA_PlayerController.cpp
``` c++
bool AAA_PlayerController::AddInventoryItem(UAA_Item* NewItem, int32 ItemCount, bool bAutoSlot)
{
	bool bChanged = false;
	if (!NewItem)
	{
		return false;
	}

	if (ItemCount <= 0)
	{
		return false;
	}

	FAA_ItemData OldData;
	GetInventoryItemData(NewItem, OldData);

	FAA_ItemData NewData = OldData;
	NewData.UpdateItemData(FAA_ItemData(ItemCount), NewItem->MaxCount);

	if (OldData != NewData)
	{
		InventoryData.Add(NewItem, NewData);
		NotifyInventoryItemChanged(true, NewItem);
		bChanged = true;
	}

	if (bAutoSlot)
	{
		bChanged |= FillEmptySlotWithItem(NewItem);
	}

	if (bChanged)
	{
		SaveInventory();
		return true;
	}

	return false;

}
```
- 아이템을 사용한 경우 해당 아이템의 FAA_ItemData의 수량이 감소합니다. 수량이 0이 되면 인벤토리에서 삭제하며 장착한 아이템이였다면 슬롯에서도 삭제합니다.
> AA_PlayerController.cpp
``` c++
bool AAA_PlayerController::RemoveInventoryItem(UAA_Item* RemovedItem, int32 RemoveCount)
{
	if (!RemovedItem)
	{
		return false;
	}

	FAA_ItemData NewData;
	GetInventoryItemData(RemovedItem, NewData);

	if (!NewData.IsValid())
	{
		return false;
	}

	// RemoveCount가 0이하라면 모두 삭제한다.
	if (RemoveCount <= 0)
	{
		NewData.ItemCount = 0;
	}
	else
	{
		NewData.ItemCount -= RemoveCount;
	}

	if (NewData.ItemCount > 0)
	{
		InventoryData.Add(RemovedItem, NewData);
	}
	else
	{
		// 모두 지웠을 경우, unslotted
		InventoryData.Remove(RemovedItem);

		for (TPair<FAA_ItemSlot, UAA_Item*>& Pair : SlottedItems)
		{
			if (Pair.Value == RemovedItem)
			{
				Pair.Value = nullptr;
				NotifySlottedItemChanged(Pair.Key, Pair.Value);
			}
		}
	}

	NotifyInventoryItemChanged(false, RemovedItem);

	SaveInventory();
	return true;
}
```
- 아이템 장착 함수는 UI에서 호출되며 해당 슬롯에 아이템을 장착합니다.
> AA_PlayerController.cpp
``` c++
bool AAA_PlayerController::SetSlottedItem(FAA_ItemSlot ItemSlot, UAA_Item* Item)
{
	bool bFound = false;
	for (TPair<FAA_ItemSlot, UAA_Item*>& Pair : SlottedItems)
	{
		if (Pair.Key == ItemSlot)
		{
			bFound = true;
			Pair.Value = Item;
			NotifySlottedItemChanged(Pair.Key, Pair.Value);
		}
		else if (Item != nullptr && Pair.Value == Item)
		{
			Pair.Value = nullptr;
			NotifySlottedItemChanged(Pair.Key, Pair.Value);
		}
	}

	if (bFound)
	{
		SaveInventory();
		return true;
	}

	return false;
}
```
> SelectEquipmentSlot.cpp
``` c++
FReply USelectEquipmentSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	AAA_PlayerController* PlayerController = Cast<AAA_PlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		if (PlayerController->GetInventoryWidget()->GetCurrentEquipmentSlot()->Item == Item)
		{
			// 아이템 장착
			PlayerController->SetSlottedItem(PlayerController->GetInventoryWidget()->GetCurrentEquipmentSlot()->EquipSlot, nullptr);
		}
		else
		{
			PlayerController->GetInventoryWidget()->GetCurrentEquipmentSlot()->UpdateEquipmentslot(Item);
		}
		
		PlayerController->GetInventoryWidget()->EndSelectEquipmentList();
	}

	return Reply;
}
```
## UI
- 기본적으로 UI가 표시할 Attribute에 Delegate에 바인딩하여 Broadcast 시 변경될 수 있게 설정하였습니다. 
> AA_HUDWidget.cpp
``` c++
PlayerCharacter->GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(PlayerAttributeSet->GetHealthAttribute()).AddUObject(this, &UAA_HUDWidget::OnPlayerHealthChanged);
```
- 아이템 유지시간을 표시하는 UItemDurationBox는 아이템 사용 시, 지속 시간의 종료까지 아이템의 GameplayTag가 부여될 수 있도록 설정하였으며, 이 GameplayTag의 Delegate를 바인딩하여 지속 시간동안 UI를 표시할 수 있도록 설정하였습니다.
> ItemDurationBox.cpp
``` c++
PlayerCharacter->GetAbilitySystemComponent()->RegisterGameplayTagEvent(
		FGameplayTag::RequestGameplayTag(FName("Potion.Damage"), EGameplayTagEventType::NewOrRemoved))
		.AddUObject(this, &UItemDurationBox::OnPotionTagChanged);
```

## 참고
- 액션 RPG 게임 | 언리얼 엔진 문서
  - https://docs.unrealengine.com/4.27/ko/Resources/SampleGames/ARPG/

- [MbM] Sekiro:Shadows Die Twice Grappling Hook | UE4
  - 갈고리 상호작용
  - https://www.youtube.com/watch?v=qaFACuEMrQs&t=1s

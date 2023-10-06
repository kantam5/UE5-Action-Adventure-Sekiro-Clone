// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/AA_AnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Characters/AA_CharacterBase.h"
#include "AA_CharacterTypes.h"
#include "DataAssets/CharacterAnimDataAsset.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"

#include "Components/CapsuleComponent.h"

UAA_AnimInstance::UAA_AnimInstance()
	: Velocity(FVector())
	, Speed(0.f)
	, Forward(0.f)
	, Side(0.f)
	, bIsMoving(false)
	, bIsCrouching(false)
	, bIsArmed(false)
	, bUpperBody(false)
	, FootTraceDistance(0.f)
	, FootLeftDistance(0.f)
	, FootRightDistance(0.f)
	, IKHipOffset(0.f)
	, RRot(FRotator())
	, RIK(0.f)
	, LRot(FRotator())
	, LIK(0.f)
	, bIsGroggying(false)
{}

void UAA_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	APawn* Pawn = TryGetPawnOwner();
	if (Pawn)
	{
		OwnerCharacter = Cast<AAA_CharacterBase>(Pawn);
		MovementComponent = Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent());
	}

	if (OwnerCharacter.Get())
	{
		FootTraceDistance = OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}
}

void UAA_AnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (OwnerCharacter == nullptr)
	{
		return;
	}

	Velocity = MovementComponent->Velocity;
	Speed = Velocity.Length();
	Forward = OwnerCharacter->GetActorRotation().UnrotateVector(OwnerCharacter->GetVelocity()).X;
	Side = OwnerCharacter->GetActorRotation().UnrotateVector(OwnerCharacter->GetVelocity()).Y;
	bIsMoving = Speed > 1.0;
	bIsInAir = MovementComponent->IsFalling();
	bIsCrouching = MovementComponent->IsCrouching();
	bIsJumping = (Velocity.Z > 100.f) && bIsInAir && OwnerCharacter->JumpCurrentCount != 2;
	bIsGuarding = OwnerCharacter->IsGuarding();
	bIsArmed = OwnerCharacter->GetIsArmed();

	SetFootPlacement();
}

void UAA_AnimInstance::PlayHitReactMontage(EHitReactDirection HitReactDirection, float DamageAmount, bool IsStaggered)
{
	UAnimMontage* CurrentHitReactMontage = nullptr;

	FOnMontageBlendingOutStarted DeflectMontageCompleteDelegate;
	FOnMontageEnded HitReactMontageCompleteDelegate;

	if (OwnerCharacter->IsAlive())
	{
		FCharacterData Data = OwnerCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset)
		{
			if (OwnerCharacter->IsDeflecting())
			{
				int32 PlayIndex = FMath::RandRange(1, Data.CharacterAnimDataAsset->CharacterAnimationData.ParryMontage.Num()) - 1;
				CurrentHitReactMontage = Data.CharacterAnimDataAsset->CharacterAnimationData.ParryMontage[PlayIndex];

				DeflectMontageCompleteDelegate.Unbind();
				DeflectMontageCompleteDelegate.BindUObject(this, &UAA_AnimInstance::OnDeflectMontageComplete);
				
			}
			else if (OwnerCharacter->IsGuarding())
			{
				int32 PlayIndex = FMath::RandRange(1, Data.CharacterAnimDataAsset->CharacterAnimationData.GuardHitMontage.Num()) - 1;
				CurrentHitReactMontage = Data.CharacterAnimDataAsset->CharacterAnimationData.GuardHitMontage[PlayIndex];
			}
			else if (IsStaggered)
			{
				if (DamageAmount > 4.f)
				{
					if (HitReactDirection == EHitReactDirection::Front)
					{
						CurrentHitReactMontage = Data.CharacterAnimDataAsset->CharacterAnimationData.FrontHeavyHitMontage;
					}
					else if (HitReactDirection == EHitReactDirection::Back)
					{
						CurrentHitReactMontage = Data.CharacterAnimDataAsset->CharacterAnimationData.BackHeavyHitMontage;
					}
				}
				else if (HitReactDirection == EHitReactDirection::Left)
				{
					int32 PlayIndex = FMath::RandRange(1, Data.CharacterAnimDataAsset->CharacterAnimationData.LeftHitMontage.Num()) - 1;
					CurrentHitReactMontage = Data.CharacterAnimDataAsset->CharacterAnimationData.LeftHitMontage[PlayIndex];
				}
				else if (HitReactDirection == EHitReactDirection::Front)
				{
					int32 PlayIndex = FMath::RandRange(1, Data.CharacterAnimDataAsset->CharacterAnimationData.FrontHitMontage.Num()) - 1;
					CurrentHitReactMontage = Data.CharacterAnimDataAsset->CharacterAnimationData.FrontHitMontage[PlayIndex];
				}
				else if (HitReactDirection == EHitReactDirection::Right)
				{
					int32 PlayIndex = FMath::RandRange(1, Data.CharacterAnimDataAsset->CharacterAnimationData.RightHitMontage.Num()) - 1;
					CurrentHitReactMontage = Data.CharacterAnimDataAsset->CharacterAnimationData.RightHitMontage[PlayIndex];
				}
				else if (HitReactDirection == EHitReactDirection::Back)
				{
					int32 PlayIndex = FMath::RandRange(1, Data.CharacterAnimDataAsset->CharacterAnimationData.BackHitMontage.Num()) - 1;
					CurrentHitReactMontage = Data.CharacterAnimDataAsset->CharacterAnimationData.BackHitMontage[PlayIndex];
				}
				else
				{
					int32 PlayIndex = FMath::RandRange(1, Data.CharacterAnimDataAsset->CharacterAnimationData.FrontHitMontage.Num()) - 1;
					CurrentHitReactMontage = Data.CharacterAnimDataAsset->CharacterAnimationData.FrontHitMontage[PlayIndex];
				}
			}
		}
	}

	if (CurrentHitReactMontage)
	{
		OwnerCharacter->OnHitReactMontageStart();

		Montage_Play(CurrentHitReactMontage);

		HitReactMontageCompleteDelegate.BindUObject(this, &UAA_AnimInstance::OnHitReactMontageComplete);
		Montage_SetBlendingOutDelegate(HitReactMontageCompleteDelegate, CurrentHitReactMontage);
		
		Montage_SetEndDelegate(DeflectMontageCompleteDelegate, CurrentHitReactMontage);
	}
	
}

void UAA_AnimInstance::OnHitReactMontageComplete(UAnimMontage* AnimMontage, bool trigger)
{
	if (OwnerCharacter->IsValidLowLevel())
	{
		OwnerCharacter->OnHitReactMontageComplete();
	}
}

void UAA_AnimInstance::OnDeflectMontageComplete(UAnimMontage* AnimMontage, bool trigger)
{
	if (OwnerCharacter->IsValidLowLevel())
	{
		OwnerCharacter->OnDeflectMontageComplete();
	}
}

void UAA_AnimInstance::PlayGroggyMontage()
{
	FCharacterData Data = OwnerCharacter->GetCharacterData();
	if (Data.CharacterAnimDataAsset)
	{
		bIsGroggying = true;
		UAnimMontage* CurrentActiveMontage = GetCurrentActiveMontage();
		if (CurrentActiveMontage)
		{
			Montage_Stop(0.f);
		}
		Montage_Play(Data.CharacterAnimDataAsset->CharacterAnimationData.GroggyMontage);
		
		CurrentActiveMontage = GetCurrentActiveMontage();

		FOnMontageBlendingOutStarted CompleteDelegate;
		CompleteDelegate.BindUObject(this, &UAA_AnimInstance::OnGroggyMontageComplete);
		Montage_SetEndDelegate(CompleteDelegate, Data.CharacterAnimDataAsset->CharacterAnimationData.GroggyMontage);
	}
}

void UAA_AnimInstance::OnGroggyMontageComplete(UAnimMontage* AnimMontage, bool trigger)
{
	bIsGroggying = false;
	OwnerCharacter->OnGroggyMontageComplete();
}

void UAA_AnimInstance::PlayDeathBlowReactMontage(FGameplayTag Tag)
{
	if (OwnerCharacter->IsAlive())
	{
		FCharacterData Data = OwnerCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset)
		{
			Montage_Play(Data.CharacterAnimDataAsset->CharacterAnimationData.DeathBlowedMontageMap[Tag]);
		}
	}
}

void UAA_AnimInstance::SetFootPlacement()
{
	if (!bIsInAir && OwnerCharacter->IsAlive())
	{
		FVector FootLeftHitLocation;
		bool LeftHit = IKFootTrace(FName("Foot_l_Socket"), FootTraceDistance, FootLeftHitLocation, FootLeftDistance);

		FVector FootRightHitLocation;
		bool RightHit = IKFootTrace(FName("Foot_r_Socket"), FootTraceDistance, FootRightHitLocation, FootRightDistance);

		float HighOffset;
		if (FootLeftDistance >= FootRightDistance)
		{
			HighOffset = FootLeftDistance;
		}
		else
		{
			HighOffset = FootRightDistance;
		}

		HighOffset -= FootTraceDistance;
		HighOffset *= -1.f;

		if (LeftHit || RightHit)
		{
			IKHipOffset = FMath::FInterpTo(IKHipOffset, HighOffset, GetDeltaSeconds(), 15.f);
		}

		FVector LeftRootHitLocation;
		float LeftRootDistance;
		FVector LeftRootNormal;
		bool FootLeftRootHit = IKRootTrace(FName("Foot_l_Socket"), FName("Root"), LeftRootHitLocation, LeftRootDistance, LeftRootNormal);

		if (!bIsMoving)
		{
			if (FootLeftRootHit)
			{
				LRot = FMath::RInterpTo(LRot,
					FRotator(FMath::RadiansToDegrees((-1.f) * UKismetMathLibrary::Atan2(LeftRootNormal.X, LeftRootNormal.Z)),
						0.0f,
						FMath::RadiansToDegrees(UKismetMathLibrary::Atan2(LeftRootNormal.Y, LeftRootNormal.Z))),
					GetDeltaSeconds(),
					15.f);

				LeftRootDistance -= 110.f;
				LeftRootDistance /= (-45.f);
				LIK = FMath::FInterpTo(LIK, LeftRootDistance, GetDeltaSeconds(), 15.f);
			}

			FVector RightRootHitLocation;
			float RightRootDistance;
			FVector RightRootNormal;
			bool FootRightRootHit = IKRootTrace(FName("Foot_r_Socket"), FName("Root"), RightRootHitLocation, RightRootDistance, RightRootNormal);

			if (FootRightRootHit)
			{
				RRot = FMath::RInterpTo(RRot,
					FRotator(FMath::RadiansToDegrees((-1.f) * UKismetMathLibrary::Atan2(RightRootNormal.X, RightRootNormal.Z)),
						0.f,
						FMath::RadiansToDegrees(UKismetMathLibrary::Atan2(RightRootNormal.Y, RightRootNormal.Z))),
					GetDeltaSeconds(),
					15.f);

				RightRootDistance -= 110.f;
				RightRootDistance /= (-45.f);
				RIK = FMath::FInterpTo(RIK, RightRootDistance, GetDeltaSeconds(), 15.f);
			}
		}
		else
		{
			LRot = FRotator();
			LIK = 0.f;
			RRot = FRotator();
			RIK = 0.f;
		}

	}
	else
	{
		LRot = FRotator();
		LIK = 0.f;
		RRot = FRotator();
		RIK = 0.f;
	}
}

bool UAA_AnimInstance::IKFootTrace(FName SocketName, float TraceDistance, FVector& HitLocation, float& IKAlpha)
{
	FVector SocketLocation = OwnerCharacter->GetMesh()->GetSocketLocation(SocketName);
	FVector MeshLocation = OwnerCharacter->GetMesh()->GetComponentLocation();
	MeshLocation.Z += TraceDistance;

	FVector Start(SocketLocation.X, SocketLocation.Y, MeshLocation.Z);
	FVector End(Start - FVector(0.f, 0.f, 2 * TraceDistance));

	FHitResult HitResult = FHitResult(ForceInit);
	FCollisionQueryParams Params(TEXT("FootTrace"), false, OwnerCharacter.Get());

	bool FootTraceHited = OwnerCharacter->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_WorldStatic, Params);
	if (FootTraceHited)
	{
		HitLocation = HitResult.Location;
		IKAlpha = HitResult.Distance;

		return true;
	}

	return false;
}

bool UAA_AnimInstance::IKRootTrace(FName SocketName, FName RootSocketName, FVector& HitLocation, float& Distance, FVector& Normal)
{
	FVector SocketLocation = OwnerCharacter->GetMesh()->GetSocketLocation(SocketName);
	FVector RootLocation = OwnerCharacter->GetMesh()->GetSocketLocation(RootSocketName);

	FVector Start(SocketLocation.X, SocketLocation.Y, RootLocation.Z + 108.f);
	FVector End(SocketLocation.X, SocketLocation.Y, RootLocation.Z - 108.f);

	FHitResult HitResult = FHitResult(ForceInit);
	FCollisionQueryParams Params(TEXT("RootTrace"), false, OwnerCharacter.Get());
	bool RootTraceHited = OwnerCharacter->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_WorldStatic, Params);
	if (RootTraceHited)
	{
		HitLocation = HitResult.Location;
		Distance = HitResult.Distance;
		Normal = HitResult.Normal;

		return true;
	}
	else
	{
		Distance = 999.f;

		return false;
	}
}

UBlendSpace* UAA_AnimInstance::GetLocomotionBlendspace() const
{
	if (OwnerCharacter.Get())
	{
		FCharacterData Data = OwnerCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.MovementBlendspace;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.MovementBlendspace : nullptr;
}

UAnimSequenceBase* UAA_AnimInstance::GetIdleAnimation() const
{
	if (OwnerCharacter.Get())
	{
		FCharacterData Data = OwnerCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset : nullptr;

}

UBlendSpace* UAA_AnimInstance::GetCrouchLocomotionBlendspace() const
{
	if (OwnerCharacter.Get())
	{
		FCharacterData Data = OwnerCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.CrouchMovementBlendspace;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchMovementBlendspace : nullptr;

}

UAnimSequenceBase* UAA_AnimInstance::GetCrouchIdleAnimation() const
{
	if (OwnerCharacter.Get())
	{
		FCharacterData Data = OwnerCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationAsset;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationAsset : nullptr;

}

UBlendSpace* UAA_AnimInstance::GetGuardLocomotionBlendspace() const
{
	if (OwnerCharacter.Get())
	{
		FCharacterData Data = OwnerCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.GuardMovementBlendspace;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.GuardMovementBlendspace : nullptr;

}

UAnimSequenceBase* UAA_AnimInstance::GetGuardIdleAnimation() const
{
	if (OwnerCharacter.Get())
	{
		FCharacterData Data = OwnerCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.GuardIdleAnimationAsset;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.GuardIdleAnimationAsset : nullptr;

}

UBlendSpace* UAA_AnimInstance::GetUnArmedLocomotionBlendspace() const
{
	if (OwnerCharacter.Get())
	{
		FCharacterData Data = OwnerCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.UnarmedMovementBlendspace;
		}
	}
	
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.UnarmedMovementBlendspace : nullptr;

}

UAnimSequenceBase* UAA_AnimInstance::GetUnArmedIdleAnimation() const
{
	if (OwnerCharacter.Get())
	{
		FCharacterData Data = OwnerCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.UnarmedIdleAnimationAsset;
		}
	}
	
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.UnarmedIdleAnimationAsset : nullptr;

}

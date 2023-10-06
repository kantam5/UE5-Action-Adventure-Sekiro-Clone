// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_LockOn.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Characters/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Characters/EnemyCharacterBase.h"


UGA_LockOn::UGA_LockOn()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGA_LockOn::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	OwnerCharacter = CastChecked<APlayerCharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	if (!OwnerCharacter.Get())
	{
		return false;
	}

	LockOnHitResult = FHitResult(ForceInit);
	FVector Start = OwnerCharacter->GetActorLocation();
	FVector End = Start + OwnerCharacter->GetFollowCamera()->GetForwardVector() * LockOnDistance;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(LockOnSphereRadius);
	FCollisionQueryParams SweepParams(TEXT("SweepParams"), false, OwnerCharacter.Get());
	bool SweepHasHit = OwnerCharacter->GetWorld()->SweepSingleByChannel(
		LockOnHitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_GameTraceChannel1,
		Sphere,
		SweepParams
	);

	// 락온 타겟이 현재 카메라에 보이는지 확인
	if (SweepHasHit)
	{
		FHitResult HitResult = FHitResult(ForceInit);
		FCollisionQueryParams TraceParams(TEXT("TraceParams"), false, OwnerCharacter.Get());
		bool TraceHasHit = OwnerCharacter->GetWorld()->LineTraceSingleByChannel(
			HitResult,
			OwnerCharacter->GetFollowCamera()->GetComponentLocation(),
			LockOnHitResult.GetActor()->GetActorLocation(),
			ECC_WorldStatic,
			TraceParams);

		if (TraceHasHit)
		{
			if (HitResult.GetActor() == LockOnHitResult.GetActor())
			{
				return true;
			}
		}
	}

	return false;
}

void UGA_LockOn::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (LockOnHitResult.bBlockingHit)
	{
		OwnerCharacter->SetLockOnTarget(Cast<AAA_CharacterBase>(LockOnHitResult.GetActor()));
		OwnerCharacter->SetIsLockOn(true);

		AEnemyCharacterBase* EnemyCharacter = Cast<AEnemyCharacterBase>(LockOnHitResult.GetActor());
		if (EnemyCharacter)
		{
			EnemyCharacter->SetIsLockedOn(true);
		}

		AAA_CharacterBase* Target = Cast<AAA_CharacterBase>(LockOnHitResult.GetActor());
		Target->OnCharacterDied.AddUniqueDynamic(OwnerCharacter.Get(), &APlayerCharacter::OnTargetDeath);

		OwnerCharacter->SetTargetCharacter(Target);
	}
}

void UGA_LockOn::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (OwnerCharacter.Get())
	{
		if (OwnerCharacter->GetLockOnTarget())
		{
			OwnerCharacter->SetLockOnTarget(nullptr);
			OwnerCharacter->SetIsLockOn(false);

			AEnemyCharacterBase* EnemyCharacter = Cast<AEnemyCharacterBase>(LockOnHitResult.GetActor());
			if (EnemyCharacter)
			{
				EnemyCharacter->SetIsLockedOn(false);
			}
			
			OwnerCharacter->SetTargetCharacter(nullptr);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


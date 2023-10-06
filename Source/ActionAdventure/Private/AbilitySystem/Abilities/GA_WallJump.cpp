// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_WallJump.h"

#include "GameFramework/Character.h"

bool UGA_WallJump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	ACharacter* OwnerCharacter = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	if (OwnerCharacter)
	{
		if (OwnerCharacter->JumpCurrentCount != 1)
		{
			return false;
		}
		
		TArray<FVector> Directions = {
			OwnerCharacter->GetActorForwardVector(),
			OwnerCharacter->GetActorForwardVector() * (-1.f),
			OwnerCharacter->GetActorRightVector(),
			OwnerCharacter->GetActorRightVector() * (-1.f),
			(OwnerCharacter->GetActorForwardVector() + OwnerCharacter->GetActorRightVector()).GetSafeNormal(),
			(OwnerCharacter->GetActorForwardVector() + OwnerCharacter->GetActorRightVector()).GetSafeNormal() * (-1.f),
			(OwnerCharacter->GetActorForwardVector() - OwnerCharacter->GetActorRightVector()).GetSafeNormal(),
			(OwnerCharacter->GetActorForwardVector() - OwnerCharacter->GetActorRightVector()).GetSafeNormal() * (-1.f)
		};

		FCollisionQueryParams TraceParams(TEXT("WallJumpTrace"), false, OwnerCharacter);
		for (FVector Direction : Directions)
		{
			FHitResult HitResult = FHitResult(ForceInit);
			FVector Start = OwnerCharacter->GetMesh()->GetComponentLocation();
			bool WallJumpTraceHited = OwnerCharacter->GetWorld()->LineTraceSingleByChannel(
				HitResult,
				Start,
				Start + Direction * TraceDistance,
				ECC_WorldStatic,
				TraceParams);

			if (WallJumpTraceHited)
			{
				return true;
			}
		}
	}

	return false;
}

void UGA_WallJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayTagContainer GameplayTagContainer = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("Event.Montage")));
	MontageTask = UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, GameplayTagContainer);
	MontageTask->OnBlendOut.AddDynamic(this, &UGA_WallJump::OnCompleted);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_WallJump::OnCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_WallJump::OnCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_WallJump::OnCancelled);
	MontageTask->EventReceived.AddDynamic(this, &UGA_WallJump::EventReceived);
	MontageTask->ReadyForActivation();

	ACharacter* OwnerCharacter = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	if (OwnerCharacter)
	{
		FVector LaunchVelocity = FVector(0.f, 0.f, 550.f);
		if (OwnerCharacter->GetVelocity().Z < 0.f)
		{
			LaunchVelocity = LaunchVelocity + FVector(0.f, 0.f, FMath::Abs(OwnerCharacter->GetVelocity().Z));
		}
		OwnerCharacter->LaunchCharacter(LaunchVelocity, false, false);

		OwnerCharacter->JumpCurrentCount++;
	}

}

void UGA_WallJump::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_WallJump::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_WallJump::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::ApplyEffect(EventTag, EventData);
}

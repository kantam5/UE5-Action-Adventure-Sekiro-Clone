// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_FireProjectile.h"

#include "Characters/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Actors/AA_Projectile.h"

void UGA_FireProjectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwningActorFromActorInfo());
	if (OwnerCharacter)
	{
		OwnerCharacter->SetUpperBodyMontage(true);
	}

	FGameplayTagContainer GameplayTagContainer = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("Event.Montage")));
	MontageTask = UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, GameplayTagContainer);
	MontageTask->OnBlendOut.AddDynamic(this, &UGA_FireProjectile::OnCompleted);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_FireProjectile::OnCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_FireProjectile::OnCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_FireProjectile::OnCancelled);
	MontageTask->EventReceived.AddDynamic(this, &UGA_FireProjectile::EventReceived);
	MontageTask->ReadyForActivation();

}

void UGA_FireProjectile::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwningActorFromActorInfo());
	if (OwnerCharacter)
	{
		OwnerCharacter->SetUpperBodyMontage(false);
	}
}

void UGA_FireProjectile::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwningActorFromActorInfo());
	if (OwnerCharacter)
	{
		OwnerCharacter->SetUpperBodyMontage(false);
	}
}

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

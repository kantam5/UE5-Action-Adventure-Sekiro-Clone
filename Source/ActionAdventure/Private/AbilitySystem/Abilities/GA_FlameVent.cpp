// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_FlameVent.h"

#include "Characters/PlayerCharacter.h"
#include "Actors/ApplyEffectActor.h"

void UGA_FlameVent::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayTagContainer GameplayTagContainer = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("Event.Montage")));
	MontageTask = UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, GameplayTagContainer);
	MontageTask->OnBlendOut.AddDynamic(this, &UGA_FlameVent::OnCompleted);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_FlameVent::OnCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_FlameVent::OnCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_FlameVent::OnCancelled);
	MontageTask->EventReceived.AddDynamic(this, &UGA_FlameVent::EventReceived);
	MontageTask->ReadyForActivation();
}

void UGA_FlameVent::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

}

void UGA_FlameVent::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

}

void UGA_FlameVent::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::ApplyEffect(EventTag, EventData);

	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.SpawnEffectActor")))
	{
		APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwningActorFromActorInfo());
		if (!IsValid(OwnerCharacter))
		{
			return;
		}

		FVector SpawnLocation = OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * Range;
		FRotator SpawnRotation = OwnerCharacter->GetActorForwardVector().Rotation();
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnLocation);
		SpawnTransform.SetRotation(SpawnRotation.Quaternion());

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AApplyEffectActor* ApplyEffectActor = GetWorld()->SpawnActorDeferred<AApplyEffectActor>(ApplyEffectActorClass,
			SpawnTransform,GetOwningActorFromActorInfo(), OwnerCharacter, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		ApplyEffectActor->FinishSpawning(SpawnTransform);
	}
}

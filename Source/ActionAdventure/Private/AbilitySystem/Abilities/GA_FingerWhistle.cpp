// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_FingerWhistle.h"

#include "Characters/PlayerCharacter.h"
#include "Perception/AISense_Hearing.h"
#include "Kismet/GameplayStatics.h"

void UGA_FingerWhistle::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwningActorFromActorInfo());
	if (OwnerCharacter)
	{
		OwnerCharacter->SetUpperBodyMontage(true);
	}

	FGameplayTagContainer GameplayTagContainer = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("Event.Montage")));
	MontageTask = UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, GameplayTagContainer);
	MontageTask->OnBlendOut.AddDynamic(this, &UGA_FingerWhistle::OnCompleted);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_FingerWhistle::OnCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_FingerWhistle::OnCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_FingerWhistle::OnCancelled);
	MontageTask->EventReceived.AddDynamic(this, &UGA_FingerWhistle::EventReceived);
	MontageTask->ReadyForActivation();
}

void UGA_FingerWhistle::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwningActorFromActorInfo());
	if (OwnerCharacter)
	{
		OwnerCharacter->SetUpperBodyMontage(false);
	}
}

void UGA_FingerWhistle::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

	APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwningActorFromActorInfo());
	if (OwnerCharacter)
	{
		OwnerCharacter->SetUpperBodyMontage(false);
	}
}

void UGA_FingerWhistle::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::ApplyEffect(EventTag, EventData);

	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.Whistle")))
	{
		APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetOwningActorFromActorInfo());
		if (!IsValid(OwnerCharacter))
		{
			return;
		}

		UAISense_Hearing::ReportNoiseEvent(
			OwnerCharacter->GetWorld(),
			OwnerCharacter->GetActorLocation(),
			1.f,
			OwnerCharacter,
			0.f,
			FName("FingerWhistle")
		);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_PlayerGuard.h"

#include "AbilitySystemComponent.h"
#include "Characters/PlayerCharacter.h"

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

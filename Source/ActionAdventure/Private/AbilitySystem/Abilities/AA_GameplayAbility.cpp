// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AA_GameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Characters/AA_CharacterBase.h"
#include "AbilitySystemLog.h"

UAA_GameplayAbility::UAA_GameplayAbility()
{
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Groggy")));
}

void UAA_GameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayEffectContextHandle EffectContext = ActorInfo->AbilitySystemComponent->MakeEffectContext();

	for (auto GameplayEffect : OngoingEffectsToJustApplyOnStart)
	{
		if (!GameplayEffect.Get()) continue;

		if (UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get())
		{
			FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
			if (SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				if (!ActiveGEHandle.WasSuccessfullyApplied())
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to apply startup effect! %s"), *GetNameSafe(GameplayEffect));
				}
			}
		}
	}

	// Effects is going to be used only by instant stabilities
	if (IsInstantiated())
	{
		for (auto GameplayEffect : OngoingEffectsToRemoveOnEnd)
		{
			if (!GameplayEffect.Get()) continue;

			if (UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get())
			{
				FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
				if (SpecHandle.IsValid())
				{
					FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					if (!ActiveGEHandle.WasSuccessfullyApplied())
					{
						UE_LOG(LogTemp, Error, TEXT("Failed to apply runtime effect! %s"), *GetNameSafe(GameplayEffect));
					}
					else
					{
						RemoveOnEndEffectHandles.Add(ActiveGEHandle);
					}
				}
			}
		}
	}
}

void UAA_GameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsInstantiated())
	{
		for (FActiveGameplayEffectHandle ActiveEffectHandle : RemoveOnEndEffectHandles)
		{
			if (ActiveEffectHandle.IsValid())
			{
				ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveEffectHandle);
			}
		}

		RemoveOnEndEffectHandles.Empty();

		FGameplayEffectContextHandle EffectContext = ActorInfo->AbilitySystemComponent->MakeEffectContext();

		for (auto GameplayEffect : EffectsToJustApplyOnEnd)
		{
			if (!GameplayEffect.Get()) continue;

			if (UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get())
			{
				FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
				if (SpecHandle.IsValid())
				{
					FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					if (!ActiveGEHandle.WasSuccessfullyApplied())
					{
						UE_LOG(LogTemp, Error, TEXT("Failed to apply startup effect! %s"), *GetNameSafe(GameplayEffect));
					}
				}
			}

		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

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

AAA_CharacterBase* UAA_GameplayAbility::GetActionGameCharacterFromActorInfo() const
{
	return Cast<AAA_CharacterBase>(GetAvatarActorFromActorInfo());
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/AbilityTypes.h"
#include "AA_GameplayAbility.generated.h"

class AAA_CharacterBase;

UCLASS()
class ACTIONADVENTURE_API UAA_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UAA_GameplayAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

	UFUNCTION(BlueprintCallable, Category = "Ability", meta = (AutoCreateRefTerm = "EventData"))
	virtual TArray<FGameplayEffectSpecHandle> MakeEffectSpec(FGameplayTag Tag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);
	
	UFUNCTION(BlueprintCallable, Category = "Ability")
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectSpec(TArray<FGameplayEffectSpecHandle>& Spec, const FGameplayEventData& EventData);

	UFUNCTION(BlueprintCallable, Category = "Ability", meta = (AutoCreateRefTerm = "EventData"))
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffect(FGameplayTag Tag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);
	
	FORCEINLINE AAA_CharacterBase* GetActionGameCharacterFromActorInfo() const;

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

};

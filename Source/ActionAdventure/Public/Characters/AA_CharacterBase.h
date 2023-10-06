// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "AA_CharacterTypes.h"
#include "Actors/WeaponActor.h"
#include "AA_CharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, AAA_CharacterBase*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterAttackDelegate);

class UAA_GameplayAbility;
class UAA_AbilitySystemComponent;
class UAA_AttributeSetBase;
class UGameplayEffect;
class UCharacterDataAsset;
class AWeaponActor;
class UAA_AnimInstance;
class UMotionWarpingComponent;
class UFootStepsComponent;

UCLASS()
class ACTIONADVENTURE_API AAA_CharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAA_CharacterBase(const FObjectInitializer& ObjectInitializer);

	FCharacterDiedDelegate OnCharacterDied;

	FCharacterAttackDelegate OnCharacterAttack;

	UPROPERTY()
	AAA_CharacterBase* TargetCharacter;

	virtual void PostLoad() override;
	virtual void PossessedBy(AController* NewController) override;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	bool ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext);
	void GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<UAA_GameplayAbility*>& ActiveAbilities);

	bool IsAlive() const;
	virtual void Die();

	virtual void HandlePoiseDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, AAA_CharacterBase* InstigatorCharacter, AActor* DamageCauser);
	void PlayHitSoundParticleEffect(AAA_CharacterBase* InstigatorCharacter, float DamageAmount);
	virtual void EndDamaged(const struct FGameplayTagContainer& DamageTags, AAA_CharacterBase* InstigatorCharacter, AActor* DamageCauser);
	virtual void OnHitReactMontageStart();
	virtual void OnHitReactMontageComplete();
	virtual void OnDeflectMontageComplete();
	virtual void OnGroggyMontageComplete();

	bool IsGuarding() const;
	bool GuardCheck(const AActor* OtherActor) const;
	bool IsDeflecting() const;
	bool DeflectCheck(const AActor* OtherActor) const;

	FORCEINLINE bool IsEnemy(int32 TeamNumber) const;

	FORCEINLINE bool IsInvincible() const;

	FORCEINLINE UAA_AttributeSetBase* GetAttributeSet() const { return AttributeSet; }

	FORCEINLINE FCharacterData GetCharacterData() const { return CharacterData; }

	FORCEINLINE void SetCharacterData(const FCharacterData& InCharacterData);

	FORCEINLINE AWeaponActor* GetWeapon() const { return Weapon; }

	FORCEINLINE UAA_AnimInstance* GetAniminstance() const { return AnimInstance; }

	FORCEINLINE UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpingComponent; }
	FORCEINLINE UFootStepsComponent* GetFootStepsComponent() const { return FootStepsComponent; }

	FORCEINLINE bool GetIsArmed() const { return bIsArmed; }
	FORCEINLINE void SetIsArmed(bool InState) { bIsArmed = InState; }

	FORCEINLINE int32 GetTeamNumber() const { return TeamNumber; }

	FORCEINLINE AAA_CharacterBase* GetTargetCharacter() const { return TargetCharacter; }
	FORCEINLINE void SetTargetCharacter(AAA_CharacterBase* InCharacter) { TargetCharacter = InCharacter; }

protected:
	UPROPERTY(EditDefaultsOnly)
	UAA_AbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY(Transient)
	UAA_AttributeSetBase* AttributeSet;

	UPROPERTY(ReplicatedUsing = OnRep_CharacterData)
	FCharacterData CharacterData;
	UPROPERTY(EditDefaultsOnly)
	UCharacterDataAsset* CharacterDataAsset;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWeaponActor> WeaponClass;
	UPROPERTY()
	AWeaponActor* Weapon;
	UPROPERTY(EditDefaultsOnly)
	FName WeaponSocket;

	UPROPERTY()
	UAA_AnimInstance* AnimInstance;
	UPROPERTY(EditDefaultsOnly)
	UMotionWarpingComponent* MotionWarpingComponent;

	UPROPERTY(EditDefaultsOnly)
	UFootStepsComponent* FootStepsComponent;

	FGameplayTag GroggyTag;
	FGameplayTag DeathBlowedTag;
	FGameplayTag DeadTag;

	bool bIsArmed;
	int32 TeamNumber;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> ResetAttributeEffect;
	
	virtual void BeginPlay() override;

	void GiveStartupAbilities();
	void ApplyStartupEffects();

	virtual void InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication = false);

	virtual void OnHealthAttributeChanged(const FOnAttributeChangeData& Data);
	virtual void OnHealthDepleted();

	virtual void OnStaminaAttributeChanged(const FOnAttributeChangeData& Data);
	virtual void OnStaminaDepleted();

	virtual void OnDeathCountAttributeChanged(const FOnAttributeChangeData& Data);
	virtual void OnDeathCountDepleted();

	void ChangeStaminaRegenRate(float Ratio);

	UFUNCTION()
	void OnRep_CharacterData();
	virtual void OnRep_PlayerState() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AA_AttributeSetBase.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class ACTIONADVENTURE_API UAA_AttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UAA_AttributeSetBase();

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAA_AttributeSetBase, Health)
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAA_AttributeSetBase, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UAA_AttributeSetBase, Stamina)
	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UAA_AttributeSetBase, MaxStamina)
	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_StaminaRegenRate)
	FGameplayAttributeData StaminaRegenRate;
	ATTRIBUTE_ACCESSORS(UAA_AttributeSetBase, StaminaRegenRate)
	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_MaxStaminaRegenRate)
	FGameplayAttributeData MaxStaminaRegenRate;
	ATTRIBUTE_ACCESSORS(UAA_AttributeSetBase, MaxStaminaRegenRate)

	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAA_AttributeSetBase, Mana)
	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAA_AttributeSetBase, MaxMana)

	UPROPERTY(BlueprintReadOnly, Category = "MovementSpeed", ReplicatedUsing = OnRep_MaxMovementSpeed)
	FGameplayAttributeData MaxMovementSpeed;
	ATTRIBUTE_ACCESSORS(UAA_AttributeSetBase, MaxMovementSpeed)

	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_AttackPower)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UAA_AttributeSetBase, AttackPower)

	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_HealthDefensePower)
	FGameplayAttributeData HealthDefensePower;
	ATTRIBUTE_ACCESSORS(UAA_AttributeSetBase, HealthDefensePower)
	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_StaminaDefensePower)
	FGameplayAttributeData StaminaDefensePower;
	ATTRIBUTE_ACCESSORS(UAA_AttributeSetBase, StaminaDefensePower)

	UPROPERTY(BlueprintReadOnly, Category = "Poise", ReplicatedUsing = OnRep_Poise)
	FGameplayAttributeData Poise;
	ATTRIBUTE_ACCESSORS(UAA_AttributeSetBase, Poise)
	UPROPERTY(BlueprintReadOnly, Category = "PoiseDamage")
	FGameplayAttributeData PoiseDamage;
	ATTRIBUTE_ACCESSORS(UAA_AttributeSetBase, PoiseDamage)

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UAA_AttributeSetBase, Damage)
	UPROPERTY(BlueprintReadOnly, Category = "StaminaDamage")
	FGameplayAttributeData StaminaDamage;
	ATTRIBUTE_ACCESSORS(UAA_AttributeSetBase, StaminaDamage)

	UPROPERTY(BlueprintReadOnly, Category = "DeathCount", ReplicatedUsing = OnRep_DeathCount)
	FGameplayAttributeData DeathCount;
	ATTRIBUTE_ACCESSORS(UAA_AttributeSetBase, DeathCount)
	UPROPERTY(BlueprintReadOnly, Category = "DeathCount", ReplicatedUsing = OnRep_MaxDeathCount)
	FGameplayAttributeData MaxDeathCount;
	ATTRIBUTE_ACCESSORS(UAA_AttributeSetBase, MaxDeathCount)

protected:
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina);
	UFUNCTION()
	virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);
	UFUNCTION()
	virtual void OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate);
	UFUNCTION()
	virtual void OnRep_MaxStaminaRegenRate(const FGameplayAttributeData& OldMaxStaminaRegenRate);

	UFUNCTION()
	virtual void OnRep_Mana(const FGameplayAttributeData& OldMana);
	UFUNCTION()
	virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);

	UFUNCTION()
	virtual void OnRep_MaxMovementSpeed(const FGameplayAttributeData& OldMaxMovementSpeed);

	UFUNCTION()
	virtual void OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower);

	UFUNCTION()
	virtual void OnRep_HealthDefensePower(const FGameplayAttributeData& OldHealthDefensePower);
	UFUNCTION()
	virtual void OnRep_StaminaDefensePower(const FGameplayAttributeData& OldStaminaDefensePower);

	UFUNCTION()
	virtual void OnRep_Poise(const FGameplayAttributeData& OldPoise);

	UFUNCTION()
	virtual void OnRep_DeathCount(const FGameplayAttributeData& OldDeathCount);
	UFUNCTION()
	virtual void OnRep_MaxDeathCount(const FGameplayAttributeData& OldMaxDeathCount);
};

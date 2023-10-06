// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/EffectExecutionCalculations/AA_DamageExecutionCalculation.h"
#include "AbilitySystem/AttributeSets/AA_AttributeSetBase.h"
#include "AbilitySystemComponent.h"

struct AADamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealthDefensePower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(StaminaDefensePower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(StaminaDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PoiseDamage);

	AADamageStatics()
	{
		// Capture the Target's DefensePower attribute. Do not snapshot it, because we want to use the health value at the moment we apply the execution.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAA_AttributeSetBase, HealthDefensePower, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAA_AttributeSetBase, StaminaDefensePower, Target, false);

		// Capture the Source's AttackPower. We do want to snapshot this at the moment we create the GameplayEffectSpec that will execute the damage.
		// (imagine we fire a projectile: we create the GE Spec when the projectile is fired. When it hits the target, we want to use the AttackPower at the moment
		// the projectile was launched, not when it hits).
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAA_AttributeSetBase, AttackPower, Source, true);

		// Also capture the source's raw Damage, which is normally passed in directly via the execution
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAA_AttributeSetBase, Damage, Source, true);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAA_AttributeSetBase, StaminaDamage, Source, true);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAA_AttributeSetBase, PoiseDamage, Source, true);
	}
};

static const AADamageStatics& DamageStatics()
{
	static AADamageStatics DmgStatics;
	return DmgStatics;
}

UAA_DamageExecutionCalculation::UAA_DamageExecutionCalculation()
{
	RelevantAttributesToCapture.Add(DamageStatics().HealthDefensePowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().StaminaDefensePowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().StaminaDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().PoiseDamageDef);
}

void UAA_DamageExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float HealthDefensePower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().HealthDefensePowerDef, EvaluationParameters, HealthDefensePower);
	if (HealthDefensePower == 0.0f)
	{
		HealthDefensePower = 1.0f;
	}
	float StaminaDefensePower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().StaminaDefensePowerDef, EvaluationParameters, StaminaDefensePower);
	if (StaminaDefensePower == 0.0f)
	{
		StaminaDefensePower = 1.0f;
	}

	float AttackPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackPowerDef, EvaluationParameters, AttackPower);

	float Damage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluationParameters, Damage);
	Damage += FMath::Max<float>(Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), false, -1.0f), 0.0f);

	float StaminaDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().StaminaDamageDef, EvaluationParameters, StaminaDamage);

	float PoiseDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PoiseDamageDef, EvaluationParameters, PoiseDamage);

	if (PoiseDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().PoiseDamageProperty, EGameplayModOp::Additive, PoiseDamage));
	}

	float DamageDone = Damage * AttackPower / HealthDefensePower;
	if (DamageDone > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, DamageDone));
	}

	float StaminaDamageDone = StaminaDamage * AttackPower / StaminaDefensePower;
	if (DamageDone > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().StaminaDamageProperty, EGameplayModOp::Additive, StaminaDamageDone));
	}

}

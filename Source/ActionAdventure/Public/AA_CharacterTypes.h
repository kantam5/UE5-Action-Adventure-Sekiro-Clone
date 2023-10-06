// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "AA_CharacterTypes.generated.h"

class UGameplayEffect;
class UGameplayAbility;

class UBlendSpace;
class UAnimSequenceBase;
class UCharacterAnimDataAsset;
class UHitSoundParticleDataAsset;
class UParticleSystem;

USTRUCT(BlueprintType)
struct FCharacterData
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
	TArray<TSubclassOf<UGameplayEffect>> Effects;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	UCharacterAnimDataAsset* CharacterAnimDataAsset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HitReact")
	UHitSoundParticleDataAsset* HitSoundParticleDataAsset;
};

// TODO: 플레이어와 적으로 데이터 나누기
USTRUCT(BlueprintType)
struct FCharacterAnimationData
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly)
	UBlendSpace* MovementBlendspace = nullptr;
	UPROPERTY(EditDefaultsOnly)
	UAnimSequenceBase* IdleAnimationAsset = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UBlendSpace* CrouchMovementBlendspace = nullptr;
	UPROPERTY(EditDefaultsOnly)
	UAnimSequenceBase* CrouchIdleAnimationAsset = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UBlendSpace* GuardMovementBlendspace = nullptr;
	UPROPERTY(EditDefaultsOnly)
	UAnimSequenceBase* GuardIdleAnimationAsset = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UBlendSpace* UnarmedMovementBlendspace = nullptr;
	UPROPERTY(EditDefaultsOnly)
	UAnimSequenceBase* UnarmedIdleAnimationAsset = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "HitReact")
	TArray<UAnimMontage*> LeftHitMontage;
	UPROPERTY(EditDefaultsOnly, Category = "HitReact")
	TArray<UAnimMontage*> FrontHitMontage;
	UPROPERTY(EditDefaultsOnly, Category = "HitReact")
	TArray<UAnimMontage*> RightHitMontage;
	UPROPERTY(EditDefaultsOnly, Category = "HitReact")
	TArray<UAnimMontage*> BackHitMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "HitReact")
	UAnimMontage* FrontHeavyHitMontage;
	UPROPERTY(EditDefaultsOnly, Category = "HitReact")
	UAnimMontage* BackHeavyHitMontage;

	UPROPERTY(EditDefaultsOnly, Category = "HitReact")
	TArray<UAnimMontage*> GuardHitMontage;
	UPROPERTY(EditDefaultsOnly, Category = "HitReact")
	TArray<UAnimMontage*> ParryMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Groggy")
	UAnimMontage* GroggyMontage;

	UPROPERTY(EditDefaultsOnly, Category = "DeathBlow")
	TMap<FGameplayTag, UAnimMontage*> DeathBlowMontageMap;

	UPROPERTY(EditDefaultsOnly, Category = "DeathBlow")
	TMap<FGameplayTag, UAnimMontage*> DeathBlowedMontageMap;

	UPROPERTY(EditDefaultsOnly, Category = "DeathBlow")
	UAnimMontage* PlungeMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	UAnimMontage* DeathMontage;
};

USTRUCT(BlueprintType)
struct FHitSoundParticleData
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly)
	TArray<USoundBase*> HitSounds;
	UPROPERTY(EditDefaultsOnly)
	TArray<USoundBase*> ParrySounds;
	UPROPERTY(EditDefaultsOnly)
	TArray<USoundBase*> GuardSounds;
	UPROPERTY(EditDefaultsOnly)
	TArray<USoundBase*> HeavyParrySounds;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* HitParticle;
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* GuardParticle; 
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* ParryParticle;
};

USTRUCT(BlueprintType)
struct FHookMontage : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(EditDefaultsonly)
	UAnimMontage* AnimMontage;

	// Character
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	UCurveFloat* HeightCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	UCurveFloat* SpeedCurve;

	// Hook
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	UCurveFloat* ThrowSpeedCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	UCurveFloat* PickupSpeedCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	UCurveFloat* SwingCurve;

	// Rope
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	UCurveFloat* LengthCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	UCurveFloat* SolverIterationsCurve;
};

UENUM(BlueprintType)
enum class EMovementDirectionType : uint8
{
	None UMETA(DisplayName = "None"),
	OrientToMovement UMETA(DisplayName = "OrientToMovement"),
	Strafe UMETA(DisplayName = "Strafe")
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/AA_CharacterBase.h"
#include "EnemyCharacterBase.generated.h"

class UWidgetComponent;
class UFloatingStatusBarWidget;
class AAA_AIController;

UCLASS()
class ACTIONADVENTURE_API AEnemyCharacterBase : public AAA_CharacterBase
{
	GENERATED_BODY()

public:
	AEnemyCharacterBase(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	void HandleDeathBlow(FGameplayTag Tag, const AAA_CharacterBase* Character);
	void HandleEndDeathBlow();

	virtual void Die() override;

	void SetLockOnTrackerVisibility(bool InState);
	void SetDeathBlowTrackerVisibility(bool InState);
	void SetBackstabDeathBlowTrackerVisibility(bool InState);
	void SetPlungeDeathBlowTrackerVisibility(bool InState);
	void SetStatusBarVisibility();
	 
	bool CanBeBackstabbed(const AActor* OtherActor);

	virtual void HandlePoiseDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, AAA_CharacterBase* InstigatorCharacter, AActor* DamageCauser) override;
	virtual void OnStaminaDepleted() override;
	virtual void OnGroggyMontageComplete() override;
	
	virtual void OnDeflectMontageComplete() override;

	void SetGuardCountTimer();

	UFUNCTION()
	void TryActiveGuardAbility();

	FVector GetLockOnLocation();

	FORCEINLINE bool GetCounter() const { return bCounter; }
	FORCEINLINE void SetCounter(bool InState) { bCounter = InState; }

	FORCEINLINE void SetIsLockedOn(bool InState) { bIsLockedOn = InState; SetStatusBarVisibility(); SetLockOnTrackerVisibility(InState); }

	FORCEINLINE void SetIsOverlappingUISphere(bool InState) { bIsOverlappingUISphere = InState; SetStatusBarVisibility(); }

	FORCEINLINE bool GetHasTarget() const { return bHasTarget; }
	FORCEINLINE void SetHasTarget(bool InState) { bHasTarget = InState; }
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFloatingStatusBarWidget> FloatingStatusBarClass;
	UPROPERTY()
	UFloatingStatusBarWidget* FloatingStatusBar;
	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* FloatingStatusBarWidgetComp;

	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* LockOnWidgetComp;
	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* DeathBlowWidgetComp;
	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* BackstabDeathBlowWidgetComp;
	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* PlungeDeathBlowWidgetComp;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer GuardTags;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DeflectEffect;

	bool bCanDeflect;
	bool bCounter;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxGuardCount;
	int32 GuardCount;
	FTimerHandle GuardTimerHandle;
	FTimerHandle GuardCountTimerHandle;
	FTimerHandle DeflectStateTimerHandle;

	bool bIsLockedOn;
	bool bIsOverlappingUISphere;

	bool bHasTarget;

	FTransform ResetTransform;
	bool bResetOnRest;

	UPROPERTY()
	AAA_AIController* UnPossessedAIController;


	void InitializeFloatingStatusBar();

	UFUNCTION()
	void OnRestInCheckPoint();

	virtual void ResetCharacterToOrigin();
};

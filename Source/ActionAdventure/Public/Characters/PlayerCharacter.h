// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/AA_CharacterBase.h"
#include "InputActionValue.h"
#include "Interfaces/AA_InventoryInterface.h"
#include "Components/TimeLineComponent.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USphereComponent;
class UAbilityTraversalComponent;
class UGrapplingHookComponent;
class UCableComponent;
class UGameplayEffect;
class UInputMappingContext;
class UInputAction;
class UANS_JumpSection;
class ABossCharacter;
class AEnemyCharacterBase;
class UInteractionComponent;
class UAA_Item;

UCLASS()
class ACTIONADVENTURE_API APlayerCharacter : public AAA_CharacterBase
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PossessedBy(AController* NewController);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TArray<UAA_Item*> SlottedSkills;
	int32 CurrentSkillIndex;
	bool bHasFilledSkillSlot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TArray<UAA_Item*> SlottedPotions;
	int32 CurrentPotionIndex;
	bool bHasFilledPotionSlot;

	FTimeline CloseCameraTimeline;
	FTimeline OutCameraTimeline;

	virtual void HandlePoiseDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, AAA_CharacterBase* InstigatorCharacter, AActor* DamageCauser) override;

	bool ActiveAbilityWithItem(UAA_Item* InItem);

	virtual void Die() override;

	UFUNCTION()
	void OnTargetDeath(AAA_CharacterBase* Character);

	void CancelLockOn();

	bool IsUsingMelee() const;

	void SetDeflectTimer();

	UFUNCTION()
	void PlungeSphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void BeginPlungeCollisionOverlap();
	void EndPlungeCollisionOverlap();

	void SetBackstabEnemy();
	void SetPlungeEnemy();

	void SetFinishDeathBlowState(ABossCharacter* InBoss);
	void EndFinisherDeathBlow();

	UFUNCTION()
	void StatusBarSphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void StatusBarSphereCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetUpperBodyMontage(bool InState);

	UFUNCTION()
	void OnRestInCheckPoint();

/*--------------------Getters Setters---------------------*/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE UGrapplingHookComponent* GetGrapplingHookComponent() const { return GrapplingHookComponent; }

	FORCEINLINE void SetLockOnTarget(const AAA_CharacterBase* InCharacter) { LockOnTarget = const_cast<AAA_CharacterBase*>(InCharacter); }
	FORCEINLINE AAA_CharacterBase* GetLockOnTarget() { return LockOnTarget; }
	FORCEINLINE void SetIsLockOn(bool InState) { bIsLockOn = InState; }
	FORCEINLINE bool GetIsLockOn() { return bIsLockOn; }

	FORCEINLINE bool CanBackstabDeathBlow() const { return bCanBackstab && BackstabEnemy; }
	FORCEINLINE AEnemyCharacterBase* GetBackstabEnemy() const { if (BackstabEnemy) return BackstabEnemy; return nullptr; }

	FORCEINLINE AEnemyCharacterBase* GetPlungeEnemy() const { if (PlungeEnemy) return PlungeEnemy; return nullptr; }

	FORCEINLINE bool GetEnableComboPeriod() { return bEnableComboPeriod; }
	FORCEINLINE void SetEnableComboPeriod(bool InState) { bEnableComboPeriod = InState; }
	FORCEINLINE const UANS_JumpSection* GetJumpSectionNotify() { return JumpSectionNotify; }
	FORCEINLINE void SetJumpSectionNotify(const UANS_JumpSection* InNotify) { JumpSectionNotify = InNotify; }

	FORCEINLINE void SetDeflectLevel(float InLevel) { DeflectLevel = InLevel; }
	FORCEINLINE float GetDeflectLevel() const { return DeflectLevel; }

	FORCEINLINE bool GetDeflectTimerPending() const { return bDeflectTimerPending; }

	FORCEINLINE const FTransform HookTransform() const { return Hook->GetComponentTransform(); }
	FORCEINLINE const FTransform HookIdleSocketTransform() const { return GetMesh()->GetSocketTransform("Idle_Hook_Socket"); }
	FORCEINLINE const FTransform HookSwingSocketTransform() const { return GetMesh()->GetSocketTransform("Swing_Hook_Socket"); }

	FORCEINLINE ABossCharacter* GetBoss() const { return Boss; }
	FORCEINLINE const bool GetCanFinish() const { return bCanFinish; }

	FORCEINLINE void SetInventoryUsing(bool InState) { bInventoryUsing = InState; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly)
	UAbilityTraversalComponent* AbilityTraversal;

	UPROPERTY(EditDefaultsOnly)
	UGrapplingHookComponent* GrapplingHookComponent;

	UPROPERTY(EditDefaultsOnly)
	UInteractionComponent* InteractionComponent;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Hook;
	UPROPERTY(EditDefaultsOnly)
	UCableComponent* Rope;

	UPROPERTY(EditDefaultsOnly)
	USphereComponent* PlungingDeathBlowCollision;
	UPROPERTY(EditDefaultsOnly)
	USphereComponent* BackstabDeathBlowCollision;
	UPROPERTY(EditDefaultsOnly)
	USphereComponent* StatusBarCollision;

	UPROPERTY(EditDefaultsOnly)
	USphereComponent* InteractionCollision;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer InAirTags;
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer CrouchTags;
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer SprintTags;
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer GuardTags;
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer LockOnTags;
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer HeavyAttackTags;
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer GrapplingHookTags;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> CrouchStateEffect;

	UPROPERTY()
	AAA_CharacterBase* LockOnTarget;
	bool bIsLockOn;
	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* LockOnCurve;

	UPROPERTY()
	AEnemyCharacterBase* BackstabEnemy;
	bool bCanBackstab;

	UPROPERTY()
	AEnemyCharacterBase* PlungeEnemy;

	bool bEnableComboPeriod;
	const UANS_JumpSection* JumpSectionNotify;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DeflectEffect;
	float DeflectLevel;
	bool bDeflectTimerPending;
	FTimerHandle DeflectTimerHandle;

	FVector HookHittedLoc;
	bool bHookHit;

	UPROPERTY()
	ABossCharacter* Boss;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* FinisherDeathBlowPrepMontage;
	bool bCanFinish;

	UPROPERTY(EditDefaultsOnly)
	USoundBase* InventorySound;

	UPROPERTY()
	TScriptInterface<IAA_InventoryInterface> InventorySource;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TMap<FAA_ItemSlot, FGameplayAbilitySpecHandle> SlottedAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	TMap<FAA_ItemSlot, TSubclassOf<UAA_GameplayAbility>> DefaultSlottedAbilities;

	FDelegateHandle InventoryUpdateHandle;
	FDelegateHandle InventoryLoadedHandle;

	bool bInventoryUsing;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* CloseCameraCurve;
	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* OutCameraCurve;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds);


	virtual void Landed(const FHitResult& Hit) override;
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust);
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	virtual void OnHealthDepleted() override;
	virtual void OnStaminaDepleted() override;


	void OnItemSlotChanged(FAA_ItemSlot ItemSlot, UAA_Item* Item);

	void RefreshSlottedGameplayAbilities();
	void RefreshSlottedItems();

	void AddSlottedGameplayAbilities();
	void FillSlottedAbilitySpecs(TMap<FAA_ItemSlot, FGameplayAbilitySpec>& SlottedAbilitySpecs);

	void RemoveSlottedGameplayAbilities(bool bRemoveAll);
	

	UFUNCTION(Server, Reliable)
	void JumpSectionForComboServer();
	void JumpSectionForComboServer_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void JumpSectionForComboNetMulticast();
	void JumpSectionForComboNetMulticast_Implementation();

	UFUNCTION()
	void CloseCameraTimelineProgress(float Value);
	UFUNCTION()
	void OutCameraTimelineProgress(float Value);

public:
	bool CanHookThrow() const;

	UAnimMontage* GetHookMontage();

	void LerpHookCharacterMovement(const FVector& StartLoc, const FVector& EndLoc, float Time);

	void AttachHook();
	void LerpHookTransform(const FTransform& StartTrans, const FTransform& EndTrans, float Time);
	void LerpHookRotation(const FRotator& StartRot, const FRotator& EndRot, float Alpha);
	void HookThrow(const FVector& StartLoc, const FVector& EndLoc, float Time, float NextFrameTime);
	void HookPickUp(const FVector& StartLoc, const FVector& EndLoc, float Time);

	bool CheckRopeVisibility() const;
	void ChangeRopeVisibility(bool InState);
	void SetRopeStiffness(bool InState);
	void UpdateRopeLength(float Time);
	void CheckHookCollision(const FVector& NextFrameLoc);

/*--------------------Inputs---------------------*/
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void OnJumpActionStarted(const FInputActionValue& Value);
	void OnCrouchActionStarted(const FInputActionValue& Value);
	void OnCrouchActionEnded(const FInputActionValue& Value);
	void OnSprintActionStarted(const FInputActionValue& Value);
	void OnSprintActionEnded(const FInputActionValue& Value);
	void OnGuardActionStarted(const FInputActionValue& Value);
	void OnGuardActionEnded(const FInputActionValue& Value);
	void OnLockOnActionStarted(const FInputActionValue& Value);
	void OnNormalAttackActionStarted(const FInputActionValue& Value);
	void OnHeavyAttackActionStarted(const FInputActionValue& Value);
	void OnGrapplingHookActionStarted(const FInputActionValue& Value);
	void OnInteractActionStarted(const FInputActionValue& Value);
	void OnInventoryActionStarted(const FInputActionValue& Value);
	void OnSwapSkilActionStarted(const FInputActionValue& Value);
	void OnSwapItemActionStarted(const FInputActionValue& Value);
	void OnUsePotionActionStarted(const FInputActionValue& Value);
	void OnUseSkillActionStarted(const FInputActionValue& Value);
	void OnCombatArtActionStarted(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly)
	UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* MoveAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* LookAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* JumpInputAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* CrouchInputAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* SprintInputAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* GuardInputAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* LockOnInputAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* NormalAttackInputAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* HeavyAttackInputAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* GrapplingHookInputAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* InteractInputAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* InventoryInputAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* SwapSkillAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* SwapPotionAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* UsePotionAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* UseSkillAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* CombatArtAction;
};

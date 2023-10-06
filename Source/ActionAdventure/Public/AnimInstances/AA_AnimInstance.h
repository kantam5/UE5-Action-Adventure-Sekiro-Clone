// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AA_GameTypes.h"
#include "GameplayTagContainer.h"
#include "AA_AnimInstance.generated.h"

class UCharacterMovementComponent;

class UCharacterAnimDataAsset;
class UBlendSpace;
class UAnimSequenceBase;

class AAA_CharacterBase;

UCLASS()
class ACTIONADVENTURE_API UAA_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UAA_AnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaTime) override;

	void PlayHitReactMontage(EHitReactDirection HitReactDirection, float DamageAmount, bool IsStaggered);
	UFUNCTION()
	void OnHitReactMontageComplete(class UAnimMontage* AnimMontage, bool trigger);
	UFUNCTION()
	void OnDeflectMontageComplete(class UAnimMontage* AnimMontage, bool trigger);

	void PlayGroggyMontage();
	UFUNCTION()
	void OnGroggyMontageComplete(class UAnimMontage* AnimMontage, bool trigger);

	void PlayDeathBlowReactMontage(FGameplayTag Tag);

	void SetFootPlacement();
	bool IKFootTrace(FName SocketName, float TraceDistance, FVector& HitLocation, float& IKAlpha);
	bool IKRootTrace(FName SocketName, FName RootSocketName, FVector& HitLocation, float& Distance, FVector& Normal);

	FORCEINLINE void SetIsGroggying(bool InState) { bIsGroggying = InState; }
	FORCEINLINE bool GetIsGroggying() { return bIsGroggying; }

	FORCEINLINE void SetUpperBody(bool InState) { bUpperBody = InState; }

protected:
	UPROPERTY()
	TWeakObjectPtr<AAA_CharacterBase> OwnerCharacter;

	UPROPERTY()
	UCharacterMovementComponent* MovementComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	float Forward;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	float Side;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bIsMoving;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bIsCrouching;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bIsJumping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bIsGuarding;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bIsArmed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bUpperBody;

	float FootTraceDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	float FootLeftDistance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	float FootRightDistance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	float IKHipOffset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	FRotator RRot;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	float RIK;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	FRotator LRot;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	float LIK;

	bool bIsGroggying;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	UCharacterAnimDataAsset* DefaultCharacterAnimDataAsset;

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	UBlendSpace* GetLocomotionBlendspace() const;
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	UAnimSequenceBase* GetIdleAnimation() const;

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	UBlendSpace* GetCrouchLocomotionBlendspace() const;
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	UAnimSequenceBase* GetCrouchIdleAnimation() const;

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	UBlendSpace* GetGuardLocomotionBlendspace() const;
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	UAnimSequenceBase* GetGuardIdleAnimation() const;

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	UBlendSpace* GetUnArmedLocomotionBlendspace() const;
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	UAnimSequenceBase* GetUnArmedIdleAnimation() const;


};

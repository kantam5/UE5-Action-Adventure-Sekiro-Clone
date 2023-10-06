// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AA_CharacterTypes.h"
#include "GameplayTagContainer.h"
#include "AA_CharacterMovementComponent.generated.h"

UCLASS()
class ACTIONADVENTURE_API UAA_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UAA_CharacterMovementComponent();

	virtual float GetMaxSpeed() const override;

	void StartSpeedUp();
	void StopSpeedUp();

	void StartSpeedDown();
	void StopSpeedDown();

	FORCEINLINE EMovementDirectionType GetMovementDirectionType() { return MovementDirectionType; }
	FORCEINLINE void SetMovementDirectionType(EMovementDirectionType InMovementDirectionType);

protected:
	UPROPERTY(EditAnywhere)
	EMovementDirectionType MovementDirectionType;

	bool RequestToSpeedUp;
	bool RequestToSpeedDown;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnEnforcedStrafeTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION()
	void OnTagChangedUpdateRotationRate(const FGameplayTag CallbackTag, int32 NewCount);

	void HandleMovementDirection();

};

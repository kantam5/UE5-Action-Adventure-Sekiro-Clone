// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_PlungeToTarget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API UAT_PlungeToTarget : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UAT_PlungeToTarget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAT_PlungeToTarget* PlungeToTarget(UGameplayAbility* OwningAbility, FName TaskInstanceName, AActor* Target);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

private:
	UPROPERTY()
	AActor* Target;

	bool bIsFinished;
};

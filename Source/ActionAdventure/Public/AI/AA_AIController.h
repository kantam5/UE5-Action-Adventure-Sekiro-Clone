// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AA_AIController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPerceptionDetectDelegate, bool, HasSeenSomething, float, DetectSpeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPerceptionMissDelegate);

class UBehaviorTree;
class UBlackboardData;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class AAA_CharacterBase;
class AEnemyCharacterBase;
class USoundBase;

UCLASS()
class ACTIONADVENTURE_API AAA_AIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AAA_AIController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AISightRadius = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AISightAge = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AILoseSightRadius = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AIFieldOfView = 90.f;

	FPerceptionDetectDelegate OnPerceptionDetect;
	FPerceptionMissDelegate OnPerceptionMiss;

	static const FName HomePosKey;
	static const FName TargetKey;
	static const FName MoveLocationKey;
	static const FName MoveLocationKey;
	static const FName DeflectKey;
	static const FName InvestigatingKey;
	static const FName NoiseLocationKey;
	static const FName InvestigatingByFingerWhistleKey;
	static const FName SecondPhaseKey;

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnTargetUpdated(AActor* Actor, FAIStimulus Stimulus);
	UFUNCTION()
	void OnHearingUpdated(const TArray<AActor*>& UpdatedActors);
	UFUNCTION()
	void OnTargetDied(AAA_CharacterBase* TargetCharacter);

	void SeenPlayer();
	void LostPlayer();

	void SetSecondPhase();

	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true);

	FORCEINLINE AAA_CharacterBase* GetTarget() const { return Target; }
	FORCEINLINE void SetTarget(AAA_CharacterBase* InTarget) { Target = InTarget; }

protected:
	TWeakObjectPtr<AEnemyCharacterBase> OwnerCharacter;

	UPROPERTY(EditAnywhere)
	UBehaviorTree* BTAsset;
	UPROPERTY(EditAnywhere)
	UBlackboardData* BBAsset;

	float SmoothFocusInterpSpeed;

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;
	UPROPERTY()
	UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY()
	AAA_CharacterBase* Target;

	UPROPERTY(EditDefaultsOnly)
	USoundBase* DetectSound;


};

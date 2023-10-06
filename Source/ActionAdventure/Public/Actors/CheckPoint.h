// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/InteractionActorBase.h"
#include "CheckPoint.generated.h"

class UNiagaraComponent;
class UBillboardComponent;
class UArrowComponent;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API ACheckPoint : public AInteractionActorBase
{
	GENERATED_BODY()
	
public:
	ACheckPoint();

	UPROPERTY(EditAnywhere)
	FText IdolName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor_Var")
	float LineTraceLength;

	FTransform SpawnTransform;

	virtual bool OnInteract(AActor* Caller) override;

	void Rest();

	void Travel();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere);
	UNiagaraComponent* NiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBillboardComponent* SpawnPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* DirectionalArrow;


	void CalculateSpawnTransform();
};

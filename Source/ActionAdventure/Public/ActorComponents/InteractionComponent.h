// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class APlayerCharacter;
class AInteractionActorBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONADVENTURE_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractionComponent();

	bool TryInteract();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCollisionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	virtual void BeginPlay() override;

private:
	TWeakObjectPtr<APlayerCharacter> OwnerCharacter;

	UPROPERTY(VisibleAnywhere)
	TArray<AInteractionActorBase*> InteractionActors;

	UPROPERTY()
	AInteractionActorBase* FocusedActor;

	void UpdateInteractionActors();

	bool IsActorVisible(AInteractionActorBase* InteractionActor) const;
	void CalcPointFacingRate(AInteractionActorBase* InteractionActor, TMap<int, float>& ActorsFacingRate) const;
	void CalcBestFacingPointRate(TMap<int, float>& ActorsFacingRate);

};

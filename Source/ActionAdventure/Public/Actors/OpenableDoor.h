// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/InteractionActorBase.h"
#include "Components/TimeLineComponent.h"
#include "OpenableDoor.generated.h"

class UAA_KeyItem;

/**
 * 
 */
UCLASS()
class ACTIONADVENTURE_API AOpenableDoor : public AInteractionActorBase
{
	GENERATED_BODY()
	
public:
	AOpenableDoor();

	virtual bool OnInteract(AActor* Caller) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* LeftDoorMeshComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* RightDoorMeshComponent;

	UPROPERTY(EditAnywhere)
	bool bNeedKey;

	UPROPERTY(VisibleAnywhere)
	bool bOpend;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAA_KeyItem* KeyItemType;

	FTimeline OpenDoorTimeline;
	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* OpenDoorCurve;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds);

	void OpenDoor();

	UFUNCTION()
	void OpenDoorTimelineProgress(float Value);

	virtual void OnActorLoaded_Implementation() override;
};

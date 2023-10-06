// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractionInterface.h"
#include "Interfaces/SaveGameInterface.h"
#include "InteractionActorBase.generated.h"

class UStartFocusedWidget;
class UWidgetComponent;

UCLASS()
class ACTIONADVENTURE_API AInteractionActorBase : public AActor, public IInteractionInterface, public ISaveGameInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractionActorBase();

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* OnScreenBoundsWidgetComp;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UStartFocusedWidget> StartFocusedWidgetClass;
	UPROPERTY()
	UStartFocusedWidget* StartFocusedWidget;
	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* StartFocusedWidgetComp;
	UPROPERTY(EditAnywhere)
	FText FocusedText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScreenMarginOffset;

	FVector2D ScreenPos;
	FVector2D FixedScreenPos;

	bool bOutScreenBounds;
	bool bFocused;

	bool bCanInteract;

	void CustomUpdate(AActor* Character);
	void SetCharacterFocused(bool InState);

	void ConstrainScreenBounds();

	virtual void OnScreenBounds() override;
	virtual void OnOutScreenBounds() override;
	virtual void StartFocus() override;
	virtual void EndFocus() override;
	virtual bool OnInteract(AActor* Caller) override;

	bool CanInteract() const;

	void HideWidget();

	void InitializeFocusedWidget();


};

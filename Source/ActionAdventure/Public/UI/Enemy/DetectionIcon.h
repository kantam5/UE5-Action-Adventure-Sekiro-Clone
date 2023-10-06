// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Characters/EnemyCharacterBase.h"
#include "DetectionIcon.generated.h"

class UCanvasPanel;
class UProgressBar;
class EnemyCharacterBase;
class AAA_AIController;

UCLASS()
class ACTIONADVENTURE_API UDetectionIcon : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	void CustomInit(AEnemyCharacterBase* InCharacter);

	UFUNCTION()
	void SightRegistered(bool bHasSeen, float InDetectSpeed);

	void HideWidget();

	UFUNCTION()
	void InitDetectWidget();

	void SetDetectionIconDetected();

	FORCEINLINE void SetOwner(AEnemyCharacterBase* InCharacter) { OwnerCharacter = InCharacter; }

protected:
	TWeakObjectPtr<AEnemyCharacterBase> OwnerCharacter;

	UPROPERTY()
	AAA_CharacterBase* Player;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* CanvasPanel;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar;

	AAA_AIController* OwnerController;

	bool bTick;
	bool bSeen;

	float DetectValuePercent;
	float DetectionSpeed;

	FTimerHandle WidgetVisibilityTimerHandle;
	FTimerHandle SeenPlayerTimerHandle;

	void EvaluateSightDetection();


};

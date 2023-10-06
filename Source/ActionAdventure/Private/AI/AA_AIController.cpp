// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AA_AIController.h"

#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionComponent.h"
#include "Characters/AA_CharacterBase.h"
#include "Characters/EnemyCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName AAA_AIController::HomePosKey(TEXT("HomePos"));
const FName AAA_AIController::TargetKey(TEXT("Target"));
const FName AAA_AIController::MoveLocationKey(TEXT("MoveLocation"));
const FName AAA_AIController::DeflectKey(TEXT("IsDeflect"));
const FName AAA_AIController::InvestigatingKey(TEXT("IsInvestigating"));
const FName AAA_AIController::NoiseLocationKey(TEXT("NoiseLocation"));
const FName AAA_AIController::InvestigatingByFingerWhistleKey(TEXT("IsInvestigatingByFingerWhistle"));
const FName AAA_AIController::SecondPhaseKey(TEXT("IsSecondPhase"));

AAA_AIController::AAA_AIController()
	: AISightRadius(1000.f)
	, AISightAge(5.f)
	, AILoseSightRadius(1000.f)
	, AIFieldOfView(90.f)
	, SmoothFocusInterpSpeed(180.f)
{
	SetPerceptionComponent(*CreateOptionalDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception")));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = AISightRadius;
	SightConfig->LoseSightRadius = AILoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = AIFieldOfView;
	SightConfig->SetMaxAge(AISightAge);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AAA_AIController::OnTargetUpdated);
	
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 3000.f;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

	GetPerceptionComponent()->ConfigureSense(*HearingConfig);
	// GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AAA_AIController::OnHearingUpdated);

}

void AAA_AIController::BeginPlay()
{
	Super::BeginPlay();

	if (GetPerceptionComponent())
	{
		UE_LOG(LogTemp, Warning, TEXT("All Systems Set"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Some Problem Occured"));
	}
}

void AAA_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	OwnerCharacter = Cast<AEnemyCharacterBase>(GetPawn());

	UBlackboardComponent* BlackboardComponent = Blackboard;
	if (UseBlackboard(BBAsset, BlackboardComponent))
	{
		if (!RunBehaviorTree(BTAsset))
		{
			UE_LOG(LogTemp, Warning, TEXT("AIController couldn't run behavior tree!"));
		}
	}
}

void AAA_AIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (OwnerCharacter.Get())
	{
		if (OwnerCharacter.Get()->IsAlive())
		{
			FActorPerceptionBlueprintInfo Info;
			GetAIPerceptionComponent()->GetActorsPerception(UGameplayStatics::GetPlayerPawn(GetWorld(), 0), Info);
			if (Info.LastSensedStimuli.Num() > 0)
			{
				// 플레이어를 놓쳤다면 위젯을 초기화
				const FAIStimulus Stimulus = Info.LastSensedStimuli[0];
				if (Stimulus.IsExpired())
				{
					OnPerceptionMiss.Broadcast();
				}
			}

			if (Target)
			{
				FRotator CurrentControllerRotation = GetControlRotation();
				FVector TargetLocation = Target->GetActorLocation();
				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(OwnerCharacter->GetActorLocation(), TargetLocation);
				FRotator InterpolateRotation = FMath::RInterpTo(CurrentControllerRotation, LookAtRotation, DeltaSeconds, 40.f);
				FRotator NewRotation = FRotator(CurrentControllerRotation.Pitch, LookAtRotation.Yaw, CurrentControllerRotation.Roll);
				// OwnerCharacter->SetActorRotation(NewRotation);
				SetControlRotation(NewRotation);
			}
		}
	}
	
	
}

void AAA_AIController::OnTargetUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.Tag == FName("Noise"))
	{
		if (Actor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
		{
			GetBlackboardComponent()->SetValueAsBool(AAA_AIController::InvestigatingKey, true);
			GetBlackboardComponent()->SetValueAsVector(AAA_AIController::NoiseLocationKey, Stimulus.StimulusLocation);
		}
	}
	else if (Stimulus.Tag == FName("FingerWhistle"))
	{
		GetBlackboardComponent()->SetValueAsBool(AAA_AIController::InvestigatingByFingerWhistleKey, true);
		GetBlackboardComponent()->SetValueAsVector(AAA_AIController::NoiseLocationKey, Stimulus.StimulusLocation);
	}
	else
	{
		if (Actor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
		{
			OnPerceptionDetect.Broadcast(Stimulus.WasSuccessfullySensed() && Actor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0), 0.0f);
		}
	}
	
}

void AAA_AIController::OnHearingUpdated(const TArray<AActor*>& UpdatedActors)
{
	for (AActor* Actor : UpdatedActors)
	{
		FActorPerceptionBlueprintInfo Info;
		GetPerceptionComponent()->GetActorsPerception(Actor, Info);
		for (FAIStimulus Stim : Info.LastSensedStimuli)
		{
			if (Stim.Tag == FName("Noise"))
			{
				GetBlackboardComponent()->SetValueAsBool(AAA_AIController::InvestigatingKey, true);
			}
		}
	}
}

void AAA_AIController::OnTargetDied(AAA_CharacterBase* TargetCharacter)
{
	if (!TargetCharacter->IsAlive())
	{
		LostPlayer();
	}
}

void AAA_AIController::SeenPlayer()
{
	Target = Cast<AAA_CharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
	if (OwnerCharacter.Get()->IsValidLowLevel())
	{
		Target->OnCharacterAttack.AddUniqueDynamic(OwnerCharacter.Get(), &AEnemyCharacterBase::TryActiveGuardAbility);
		Target->OnCharacterDied.AddUniqueDynamic(this, &AAA_AIController::OnTargetDied);

		if (!OwnerCharacter.Get()->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Movement.Enforced.Strafe"))))
		{
			OwnerCharacter.Get()->GetAbilitySystemComponent()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Movement.Enforced.Strafe")));
		}
		OwnerCharacter->SetIsArmed(true);
		OwnerCharacter->SetHasTarget(true);
		OwnerCharacter->SetTargetCharacter(Target);
		
		if (DetectSound->IsValidLowLevel())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), DetectSound);
		}
		
	}
}

void AAA_AIController::LostPlayer()
{
	if (Target)
	{
		Target->OnCharacterAttack.Clear();
		Target = nullptr;
	}
	
	if (OwnerCharacter.Get())
	{
		OwnerCharacter.Get()->GetAbilitySystemComponent()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Movement.Enforced.Strafe")));
		OwnerCharacter->SetIsArmed(false);
		OwnerCharacter->SetHasTarget(false);
		OwnerCharacter->SetTargetCharacter(nullptr);
	}
	
}

void AAA_AIController::SetSecondPhase()
{
	GetBlackboardComponent()->SetValueAsBool(AAA_AIController::SecondPhaseKey, true);
}

void AAA_AIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	Super::UpdateControlRotation(DeltaTime, false);

	if (bUpdatePawn)
	{
		if (OwnerCharacter.Get())
		{
			const FRotator CurrentRot = OwnerCharacter.Get()->GetActorRotation();

			SmoothFocusInterpSpeed = OwnerCharacter.Get()->GetCharacterMovement()->RotationRate.Yaw;

			FRotator LerpTargetRot = UKismetMathLibrary::RInterpTo_Constant(OwnerCharacter.Get()->GetActorRotation(), ControlRotation, DeltaTime, SmoothFocusInterpSpeed);

			if (CurrentRot.Equals(LerpTargetRot, 1e-3f) == false)
			{
				OwnerCharacter.Get()->FaceRotation(LerpTargetRot, DeltaTime);
			}
		}
	}
}

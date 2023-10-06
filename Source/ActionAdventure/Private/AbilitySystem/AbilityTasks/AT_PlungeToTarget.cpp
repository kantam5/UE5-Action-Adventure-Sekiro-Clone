// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AT_PlungeToTarget.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UAT_PlungeToTarget::UAT_PlungeToTarget(const FObjectInitializer& ObjectInitializer)
{
	bTickingTask = true;
}

UAT_PlungeToTarget* UAT_PlungeToTarget::PlungeToTarget(UGameplayAbility* OwningAbility, FName TaskInstanceName, AActor* Target)
{
	UAT_PlungeToTarget* MyObj = NewAbilityTask<UAT_PlungeToTarget>(OwningAbility, TaskInstanceName);
	MyObj->Target = Target;

	return MyObj;

}

void UAT_PlungeToTarget::Activate()
{
	Super::Activate();

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetAvatarActor());
	if (OwnerCharacter)
	{
		OwnerCharacter->GetCharacterMovement()->Velocity.X = 0.f;
		OwnerCharacter->GetCharacterMovement()->Velocity.Y = 0.f;
	}
}

void UAT_PlungeToTarget::TickTask(float DeltaTime)
{
	if (bIsFinished)
	{
		return;
	}

	Super::TickTask(DeltaTime);

	
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetAvatarActor());
	if (OwnerCharacter)
	{
		FVector CharacterLocation = OwnerCharacter->GetActorLocation();
		FVector TargetLocation = Target->GetActorLocation();

		FRotator EndRot = UKismetMathLibrary::FindLookAtRotation(CharacterLocation, TargetLocation);
		FVector EndRotVector = EndRot.Vector();
		EndRotVector.Z *= 0.5f;

		TargetLocation.Z = CharacterLocation.Z - 300.f;
		FVector Direction = (TargetLocation - CharacterLocation).GetSafeNormal();
		OwnerCharacter->SetActorLocation(OwnerCharacter->GetActorLocation() + 30.f * EndRotVector);

	}

	FVector TargetLocation2D = FVector(Target->GetActorLocation().X, Target->GetActorLocation().Y, 0.f);
	FVector MyCharacterLocation2D = FVector(OwnerCharacter->GetActorLocation().X, OwnerCharacter->GetActorLocation().Y, 0.f);
	if (FVector::Dist(TargetLocation2D, MyCharacterLocation2D) < 30.f)
	{
		bIsFinished = true;
		EndTask();
	}
}

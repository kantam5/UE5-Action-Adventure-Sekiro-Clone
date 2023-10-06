// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/FootStepsComponent.h"

#include "Characters/PlayerCharacter.h"
#include "PhysicalMaterials/SurfacePhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Perception/AISense_Hearing.h"

UFootStepsComponent::UFootStepsComponent()
	: LeftFootSocketName(TEXT("Foot_L"))
	, RightFootSocketName(TEXT("Foot_R"))
{
	PrimaryComponentTick.bCanEverTick = false;

	FootStepVolumeMultiplier = 1.f;
}

void UFootStepsComponent::HandleFootStep(EFoot Foot)
{
	AAA_CharacterBase* OwnerCharacter = Cast<AAA_CharacterBase>(GetOwner());
	if (OwnerCharacter)
	{
		FVector SocketLocation = OwnerCharacter->GetMesh()->GetSocketLocation(Foot == EFoot::Left ? LeftFootSocketName : RightFootSocketName);

		FVector Start(SocketLocation.X, SocketLocation.Y, SocketLocation.Z + 20.f);
		FVector End(SocketLocation.X, SocketLocation.Y, SocketLocation.Z - 50.f);

		FHitResult HitResult = FHitResult(ForceInit);
		FCollisionQueryParams Params(TEXT("FootStepTrace"), false, OwnerCharacter);
		Params.bReturnPhysicalMaterial = true;
		bool FootStepTraceHited = OwnerCharacter->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_WorldStatic, Params);
		if (FootStepTraceHited)
		{
			if (HitResult.PhysMaterial.Get())
			{
				USurfacePhysicalMaterial* PhysicalMaterial = Cast<USurfacePhysicalMaterial>(HitResult.PhysMaterial.Get());
				if (PhysicalMaterial)
				{
					UGameplayStatics::PlaySoundAtLocation(this, PhysicalMaterial->FootStepSound, SocketLocation, FootStepVolumeMultiplier);
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PhysicalMaterial->FootStepVFX, SocketLocation);
				}
			}

			UAISense_Hearing::ReportNoiseEvent(
				OwnerCharacter->GetWorld(),
				SocketLocation,
				0.2f,
				OwnerCharacter,
				0.f,
				FName("Noise")
			);
		}
	}
}

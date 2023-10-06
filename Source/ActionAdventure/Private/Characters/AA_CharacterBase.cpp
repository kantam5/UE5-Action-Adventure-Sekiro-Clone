// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AA_CharacterBase.h"

#include "AbilitySystem/Components/AA_AbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/AA_AttributeSetBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "DataAssets/CharacterDataAsset.h"
#include "DataAssets/HitSoundParticleDataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ActorComponents/AA_CharacterMovementComponent.h"
#include "AA_GameTypes.h"
#include "AnimInstances/AA_AnimInstance.h"
#include "MotionWarpingComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstances/AA_GameInstance.h"
#include "ActorComponents/FootStepsComponent.h"

AAA_CharacterBase::AAA_CharacterBase(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UAA_CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
	, WeaponSocket(NAME_None)
	, bIsArmed(false)
	, TeamNumber(0)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAA_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &AAA_CharacterBase::OnHealthAttributeChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetStaminaAttribute()).AddUObject(this, &AAA_CharacterBase::OnStaminaAttributeChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetDeathCountAttribute()).AddUObject(this, &AAA_CharacterBase::OnDeathCountAttributeChanged);
	
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AttributeSet = CreateDefaultSubobject<UAA_AttributeSetBase>(TEXT("AttributeSet"));

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));

	FootStepsComponent = CreateDefaultSubobject<UFootStepsComponent>(TEXT("FootStepsComponent"));

	GroggyTag = FGameplayTag::RequestGameplayTag(FName("State.Debuff.Groggy"));
	DeathBlowedTag = FGameplayTag::RequestGameplayTag(FName("State.Debuff.DeathBlowed"));
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

void AAA_CharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (WeaponClass)
	{
		FTransform Transform;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		Weapon = GetWorld()->SpawnActorDeferred<AWeaponActor>(WeaponClass, Transform, this);
		Weapon->Init();
		Weapon->FinishSpawning(Transform);
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	}

	AnimInstance = Cast<UAA_AnimInstance>(GetMesh()->GetAnimInstance());
}

void AAA_CharacterBase::PostLoad()
{
	Super::PostLoad();

	if (IsValid(CharacterDataAsset))
	{
		SetCharacterData(CharacterDataAsset->CharacterData);
	}
}

UAbilitySystemComponent* AAA_CharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool AAA_CharacterBase::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext)
{
	if (!Effect.Get())
	{
		return false;
	}

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, InEffectContext);
	if (SpecHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		
		return ActiveGEHandle.WasSuccessfullyApplied();
	}

	return false;
}

void AAA_CharacterBase::GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<UAA_GameplayAbility*>& ActiveAbilities)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetActiveAbilitiesWithTags(AbilityTags, ActiveAbilities);
	}
}

void AAA_CharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	GiveStartupAbilities();
	ApplyStartupEffects();
}

void AAA_CharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AAA_CharacterBase::GiveStartupAbilities()
{
	if (HasAuthority() && AbilitySystemComponent)
	{
		for (auto DefaultAbility : CharacterData.Abilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DefaultAbility));
		}
	}
	
}

void AAA_CharacterBase::ApplyStartupEffects()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		for (auto CharacterEffect : CharacterData.Effects)
		{
			ApplyGameplayEffectToSelf(CharacterEffect, EffectContext);
		}
	}
	
}

void AAA_CharacterBase::OnRep_CharacterData()
{
	InitFromCharacterData(CharacterData, true);
}

void AAA_CharacterBase::InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication)
{
}

void AAA_CharacterBase::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.f && Data.OldValue > 0.f)
	{
		OnHealthDepleted();
	}

	float HealthRatio = AttributeSet->GetHealth() / AttributeSet->GetMaxHealth();
	ChangeStaminaRegenRate(HealthRatio);
}

void AAA_CharacterBase::OnHealthDepleted()
{
}

void AAA_CharacterBase::OnStaminaAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.f && Data.OldValue > 0.f)
	{
		OnStaminaDepleted();
	}

}

void AAA_CharacterBase::OnStaminaDepleted()
{
}

void AAA_CharacterBase::OnDeathCountAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	{
		OnDeathCountDepleted();
	}
}

void AAA_CharacterBase::OnDeathCountDepleted()
{
}

void AAA_CharacterBase::ChangeStaminaRegenRate(float Ratio)
{
	if (Ratio <= 0.25f)
	{
		AttributeSet->SetStaminaRegenRate(0.f);
	}
	else if (Ratio <= 0.5f)
	{
		AttributeSet->SetStaminaRegenRate(AttributeSet->GetMaxStaminaRegenRate() / 3.f);
	}
	else if (Ratio <= 0.75f)
	{
		AttributeSet->SetStaminaRegenRate((AttributeSet->GetMaxStaminaRegenRate() / 3.f) * 2.f);
	}
	else
	{
		AttributeSet->SetStaminaRegenRate(AttributeSet->GetMaxStaminaRegenRate());
	}

}

bool AAA_CharacterBase::IsAlive() const
{
	return AttributeSet->GetDeathCount() > 0.f;
}

void AAA_CharacterBase::Die()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OnCharacterDied.Broadcast(this);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAbilities();

		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

}

void AAA_CharacterBase::HandlePoiseDamage(float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags, AAA_CharacterBase* InstigatorCharacter, AActor* DamageCauser)
{
	EHitReactDirection HitReactDirection;

	const FVector& ActorLocation = GetActorLocation();
	float DistanceToFrontBackPlane = FVector::PointPlaneDist(InstigatorCharacter->GetActorLocation(), ActorLocation, GetActorRightVector());
	float DistanceToRightLeftPlane = FVector::PointPlaneDist(InstigatorCharacter->GetActorLocation(), ActorLocation, GetActorForwardVector());
	
	if (DamageAmount > 4.f)
	{
		if (DistanceToRightLeftPlane >= 0)
		{
			HitReactDirection = EHitReactDirection::Front;
		}
		else
		{
			HitReactDirection = EHitReactDirection::Back;
		}
	}
	else
	{
		if (FMath::Abs(DistanceToFrontBackPlane) <= FMath::Abs(DistanceToRightLeftPlane))
		{
			if (DistanceToRightLeftPlane >= 0)
			{
				HitReactDirection = EHitReactDirection::Front;
			}
			else
			{
				HitReactDirection = EHitReactDirection::Back;
			}
		}
		else
		{
			if (DistanceToFrontBackPlane >= 0)
			{
				HitReactDirection = EHitReactDirection::Right;
			}
			else
			{
				HitReactDirection = EHitReactDirection::Left;
			}
		}
	}

	bool IsStaggered = AttributeSet->GetPoise() - DamageAmount <= 0 ? true : false;

	if (AnimInstance)
	{
		AnimInstance->PlayHitReactMontage(HitReactDirection, DamageAmount, IsStaggered);
	}
	
	PlayHitSoundParticleEffect(InstigatorCharacter, DamageAmount);

}

void AAA_CharacterBase::OnHitReactMontageStart()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Hitted")));
	}
}

void AAA_CharacterBase::OnHitReactMontageComplete()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Hitted")));
	}
}

void AAA_CharacterBase::PlayHitSoundParticleEffect(AAA_CharacterBase* InstigatorCharacter, float DamageAmount)
{
	if (CharacterData.HitSoundParticleDataAsset)
	{
		if (DeflectCheck(InstigatorCharacter))
		{
			if (DamageAmount > 4.f)
			{
				int32 SoundIndex = FMath::RandRange(1, CharacterData.HitSoundParticleDataAsset->HitSoundParticleData.HeavyParrySounds.Num()) - 1;
				UGameplayStatics::PlaySound2D(GetWorld(), CharacterData.HitSoundParticleDataAsset->HitSoundParticleData.HeavyParrySounds[SoundIndex]);
			}
			else
			{
				int32 SoundIndex = FMath::RandRange(1, CharacterData.HitSoundParticleDataAsset->HitSoundParticleData.ParrySounds.Num()) - 1;
				UGameplayStatics::PlaySound2D(GetWorld(), CharacterData.HitSoundParticleDataAsset->HitSoundParticleData.ParrySounds[SoundIndex]);
			}
			
			FRotator EndRot = UKismetMathLibrary::FindLookAtRotation(
				GetActorLocation(),
				InstigatorCharacter->GetActorLocation());
			FVector SpawnLocation = GetActorLocation() + 50.f * EndRot.Vector();
			FTransform SpawnTransform(EndRot, SpawnLocation);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
				CharacterData.HitSoundParticleDataAsset->HitSoundParticleData.ParryParticle,
				SpawnTransform);
		}
		else if (GuardCheck(InstigatorCharacter))
		{
			int32 SoundIndex = FMath::RandRange(1, CharacterData.HitSoundParticleDataAsset->HitSoundParticleData.GuardSounds.Num()) - 1;
			UGameplayStatics::PlaySound2D(GetWorld(), CharacterData.HitSoundParticleDataAsset->HitSoundParticleData.GuardSounds[SoundIndex]);

			FRotator EndRot = UKismetMathLibrary::FindLookAtRotation(
				GetActorLocation(),
				InstigatorCharacter->GetActorLocation());
			FVector SpawnLocation = GetActorLocation() + 50.f * EndRot.Vector();
			FTransform SpawnTransform(EndRot, SpawnLocation);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
				CharacterData.HitSoundParticleDataAsset->HitSoundParticleData.GuardParticle,
				SpawnTransform);
		}
		else
		{
			int32 SoundIndex = FMath::RandRange(1, CharacterData.HitSoundParticleDataAsset->HitSoundParticleData.HitSounds.Num()) - 1;
			UGameplayStatics::PlaySound2D(GetWorld(), CharacterData.HitSoundParticleDataAsset->HitSoundParticleData.HitSounds[SoundIndex]);

			FRotator EndRot = UKismetMathLibrary::FindLookAtRotation(
				GetActorLocation(),
				InstigatorCharacter->GetActorLocation());
			FVector SpawnLocation = GetActorLocation() + 50.f * EndRot.Vector();
			FTransform SpawnTransform(EndRot, SpawnLocation);

			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
				CharacterData.HitSoundParticleDataAsset->HitSoundParticleData.HitParticle,
				SpawnTransform);
		}
	}
	
}

void AAA_CharacterBase::EndDamaged(const FGameplayTagContainer& DamageTags, AAA_CharacterBase* InstigatorCharacter, AActor* DamageCauser)
{
	if (DamageTags.HasTag(FGameplayTag::RequestGameplayTag(FName("State.Attack"))))
	{
		if (DeflectCheck(InstigatorCharacter))
		{
			// 튕겨내기를 한번 성공하면 튕겨내기 이펙트를 제거한다.
			FGameplayTagContainer GameplayTagContainer(FGameplayTag::RequestGameplayTag(FName("State.Deflect")));
			AbilitySystemComponent->RemoveActiveEffectsWithTags(GameplayTagContainer);
		}
	}
}


void AAA_CharacterBase::OnGroggyMontageComplete()
{
	if (IsAlive())
	{
		AttributeSet->SetStamina(AttributeSet->GetMaxStamina() / 3.f);

		AbilitySystemComponent->RemoveLooseGameplayTag(GroggyTag);
	}
}

bool AAA_CharacterBase::IsGuarding() const
{
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer GameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("Ability.Guard")));
		TArray<UAA_GameplayAbility*> ActiveAbilities;
		AbilitySystemComponent->GetActiveAbilitiesWithTags(GameplayTagContainer, ActiveAbilities);

		return ActiveAbilities.Num() > 0;
	}

	return false;
}

bool AAA_CharacterBase::GuardCheck(const AActor* OtherActor) const
{
	if (!IsGuarding())
	{
		return false;
	}

	float DistanceToFrontBackPlane = FVector::PointPlaneDist(OtherActor->GetActorLocation(), GetActorLocation(), GetActorRightVector());
	float DistanceToRightLeftPlane = FVector::PointPlaneDist(OtherActor->GetActorLocation(), GetActorLocation(), GetActorForwardVector());
	if (FMath::Abs(DistanceToFrontBackPlane) <= FMath::Abs(DistanceToRightLeftPlane))
	{
		if (DistanceToRightLeftPlane >= 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool AAA_CharacterBase::IsDeflecting() const
{
	FGameplayTagContainer TagContainer;
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetOwnedGameplayTags(TagContainer);

		return TagContainer.HasTag(FGameplayTag::RequestGameplayTag(FName("State.Deflect")));
	}

	return false;
}

bool AAA_CharacterBase::DeflectCheck(const AActor* OtherActor) const
{
	if (!IsDeflecting())
	{
		return false;
	}

	float DistanceToFrontBackPlane = FVector::PointPlaneDist(OtherActor->GetActorLocation(), GetActorLocation(), GetActorRightVector());
	float DistanceToRightLeftPlane = FVector::PointPlaneDist(OtherActor->GetActorLocation(), GetActorLocation(), GetActorForwardVector());
	if (FMath::Abs(DistanceToFrontBackPlane) <= FMath::Abs(DistanceToRightLeftPlane))
	{
		if (DistanceToRightLeftPlane >= 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool AAA_CharacterBase::IsEnemy(int32 InTeamNumber) const
{
	return TeamNumber != InTeamNumber;
}

void AAA_CharacterBase::SetCharacterData(const FCharacterData& InCharacterData)
{
	CharacterData = InCharacterData;

	InitFromCharacterData(InCharacterData);
}

void AAA_CharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAA_CharacterBase, CharacterData);
}

bool AAA_CharacterBase::IsInvincible() const
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Invincible")));
	}
	else
	{
		return false;
	}
}

void AAA_CharacterBase::OnDeflectMontageComplete()
{
}

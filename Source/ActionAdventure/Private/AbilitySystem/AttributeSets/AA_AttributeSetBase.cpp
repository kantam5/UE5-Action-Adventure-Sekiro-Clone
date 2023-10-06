// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AttributeSets/AA_AttributeSetBase.h"
#include "GameplayEffectExtension.h"
#include "Gameframework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Characters/AA_CharacterBase.h"
#include "Net/UnrealNetwork.h"

UAA_AttributeSetBase::UAA_AttributeSetBase()
	: AttackPower(1.0f)
	, HealthDefensePower(1.0f)
	, StaminaDefensePower(1.0f)
	, Damage(0.0f)
	, PoiseDamage(0.0f)
	, StaminaDamage(0.0f)
{
}

void UAA_AttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();

	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	AAA_CharacterBase* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<AAA_CharacterBase>(TargetActor);
	}

	AActor* SourceActor = nullptr;
	AController* SourceController = nullptr;
	AAA_CharacterBase* SourceCharacter = nullptr;
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	
	if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.Get())
	{
		SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
		SourceController = Source->AbilityActorInfo->PlayerController.Get();
		if (SourceController == nullptr && SourceActor != nullptr)
		{
			if (APawn* Pawn = Cast<APawn>(SourceActor))
			{
				SourceController = Pawn->GetController();
			}
		}

		// Use the controller to find the source pawn
		if (SourceController)
		{
			SourceCharacter = Cast<AAA_CharacterBase>(SourceController->GetPawn());
		}
		else
		{
			SourceCharacter = Cast<AAA_CharacterBase>(SourceActor);
		}

		// Set the causer actor based on context if it's set
		if (Context.GetEffectCauser())
		{
			SourceActor = Context.GetEffectCauser();
		}
	}

	if (Data.EvaluatedData.Attribute == GetPoiseDamageAttribute())
	{
		const float LocalPoiseDamageDone = GetPoiseDamage();
		SetPoiseDamage(0.f);

		// 타겟의 강인도가 공격의 강인도보다 낮을 경우 경직을 유발한다.
		bool WasAlive = true;
		if (TargetCharacter)
		{
			WasAlive = TargetCharacter->IsAlive();
		}

		if (!TargetCharacter->IsAlive())
		{
			// UE_LOG(LogTemp, Warning, TEXT("%s() %s is NOT alive when receiving damage"), TEXT(__FUNCTION__), *TargetCharacter->GetName());
		}

		if (TargetCharacter && WasAlive && SourceCharacter && GetHealth() > 0.f)
		{
			TargetCharacter->HandlePoiseDamage(LocalPoiseDamageDone, *Context.GetHitResult(), SourceTags, SourceCharacter, SourceActor);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float LocalDamageDone = GetDamage();
		SetDamage(0.f);
		
		if (!TargetCharacter->GuardCheck(SourceCharacter) && !TargetCharacter->DeflectCheck(SourceCharacter) && LocalDamageDone > 0.0f)
		{
			bool WasAlive = true;
			if (TargetCharacter)
			{
				WasAlive = TargetCharacter->IsAlive();
			}

			if (!TargetCharacter->IsAlive())
			{
				// UE_LOG(LogTemp, Warning, TEXT("%s() %s is NOT alive when receiving damage"), TEXT(__FUNCTION__), *TargetCharacter->GetName());
			}

			if (WasAlive)
			{
				const float NewHealth = GetHealth() - LocalDamageDone;
				SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaDamageAttribute())
	{
		float LocalStaminaDamage = GetStaminaDamage();
		SetStaminaDamage(0.f);

		if (LocalStaminaDamage > 0.0f)
		{
			// 가드 중이거나 튕겨내기 중이라면 스태미나 데미지를 1/3으로 감소시킨다.
			if (TargetCharacter->GuardCheck(SourceCharacter) || TargetCharacter->DeflectCheck(SourceCharacter))
			{
				LocalStaminaDamage /= 3.f;
			}

			bool WasAlive = true;
			if (TargetCharacter)
			{
				WasAlive = TargetCharacter->IsAlive();
			}

			if (!TargetCharacter->IsAlive())
			{
				// UE_LOG(LogTemp, Warning, TEXT("%s() %s is NOT alive when receiving damage"), TEXT(__FUNCTION__), *TargetCharacter->GetName());
			}

			float NewStamina = GetStamina() - LocalStaminaDamage;
			// 튕겨냈다면 그로기 상태가 되지 않는다.
			if (NewStamina <= 0.f)
			{
				if (TargetCharacter->IsDeflecting())
				{
					NewStamina = 1.f;
				}
			}

			if (WasAlive)
			{
				SetStamina(FMath::Clamp(NewStamina, 0.0f, GetMaxStamina()));
			}
			
		}

		if (TargetCharacter)
		{
			TargetCharacter->EndDamaged(SourceTags, SourceCharacter, SourceActor);
		}
		
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Handle Health changes.
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		// Handle mana changes.
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		// Handle stamina changes.
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxMovementSpeedAttribute())
	{
		ACharacter* OwningCharacter = Cast<ACharacter>(GetOwningActor());
		UCharacterMovementComponent* CharacterMovement = 
			OwningCharacter ? OwningCharacter->GetCharacterMovement() : nullptr;
		const float MaxSpeed = GetMaxMovementSpeed();

		if (CharacterMovement)
		{
			CharacterMovement->MaxWalkSpeed = MaxSpeed;
		}
	}
	else if (Data.EvaluatedData.Attribute == GetAttackPowerAttribute())
	{
		SetAttackPower(GetAttackPower());
	}
	else if (Data.EvaluatedData.Attribute == GetHealthDefensePowerAttribute())
	{
		SetHealthDefensePower(GetHealthDefensePower());
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaDefensePowerAttribute())
	{
		SetStaminaDefensePower(GetStaminaDefensePower());
	}
	else if (Data.EvaluatedData.Attribute == GetPoiseAttribute())
	{
		SetPoise(GetPoise());
	}
	else if (Data.EvaluatedData.Attribute == GetDeathCountAttribute())
	{
		SetDeathCount(FMath::Clamp(GetDeathCount(), 0.f, GetMaxDeathCount()));
	}

}

void UAA_AttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAA_AttributeSetBase, Health, OldHealth);
}

void UAA_AttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAA_AttributeSetBase, MaxHealth, OldMaxHealth);
}

void UAA_AttributeSetBase::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAA_AttributeSetBase, Stamina, OldStamina);
}

void UAA_AttributeSetBase::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAA_AttributeSetBase, MaxStamina, OldMaxStamina);
}

void UAA_AttributeSetBase::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAA_AttributeSetBase, StaminaRegenRate, OldStaminaRegenRate);
}

void UAA_AttributeSetBase::OnRep_MaxStaminaRegenRate(const FGameplayAttributeData& OldMaxStaminaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAA_AttributeSetBase, MaxStaminaRegenRate, OldMaxStaminaRegenRate);
}

void UAA_AttributeSetBase::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAA_AttributeSetBase, Mana, OldMana);
}

void UAA_AttributeSetBase::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAA_AttributeSetBase, MaxMana, OldMaxMana);
}

void UAA_AttributeSetBase::OnRep_MaxMovementSpeed(const FGameplayAttributeData& OldMaxMovementSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAA_AttributeSetBase, MaxMovementSpeed, OldMaxMovementSpeed);
}

void UAA_AttributeSetBase::OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAA_AttributeSetBase, AttackPower, OldAttackPower);
}

void UAA_AttributeSetBase::OnRep_HealthDefensePower(const FGameplayAttributeData& OldHealthDefensePower)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAA_AttributeSetBase, HealthDefensePower, OldHealthDefensePower);
}

void UAA_AttributeSetBase::OnRep_StaminaDefensePower(const FGameplayAttributeData& OldStaminaDefensePower)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAA_AttributeSetBase, StaminaDefensePower, OldStaminaDefensePower);
}

void UAA_AttributeSetBase::OnRep_Poise(const FGameplayAttributeData& OldPoise)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAA_AttributeSetBase, Poise, OldPoise);
}

void UAA_AttributeSetBase::OnRep_DeathCount(const FGameplayAttributeData& OldDeathCount)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAA_AttributeSetBase, DeathCount, OldDeathCount);
}

void UAA_AttributeSetBase::OnRep_MaxDeathCount(const FGameplayAttributeData& OldMaxDeathCount)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAA_AttributeSetBase, MaxDeathCount, OldMaxDeathCount);
}

void UAA_AttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UAA_AttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAA_AttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAA_AttributeSetBase, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAA_AttributeSetBase, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAA_AttributeSetBase, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAA_AttributeSetBase, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAA_AttributeSetBase, MaxMovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAA_AttributeSetBase, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAA_AttributeSetBase, HealthDefensePower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAA_AttributeSetBase, StaminaDefensePower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAA_AttributeSetBase, Poise, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAA_AttributeSetBase, DeathCount, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAA_AttributeSetBase, MaxDeathCount, COND_None, REPNOTIFY_Always);
}
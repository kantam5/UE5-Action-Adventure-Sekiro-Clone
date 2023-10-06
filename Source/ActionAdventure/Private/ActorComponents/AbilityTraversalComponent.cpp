// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/AbilityTraversalComponent.h"

#include "AbilitySystemComponent.h"

bool UAbilityTraversalComponent::TryJumpTraversal(UAbilitySystemComponent* ASC)
{
	for (TSubclassOf<UGameplayAbility> AbilityClass : TraversalJumpAbilitiesOrdered)
	{
		if (ASC->TryActivateAbilityByClass(AbilityClass, true))
		{
			FGameplayAbilitySpec* Spec;

			Spec = ASC->FindAbilitySpecFromClass(AbilityClass);
			if (Spec && Spec->IsActive())
			{
				return true;
			}
		}
	}

	return false;
}

bool UAbilityTraversalComponent::TryShiftTraversal(UAbilitySystemComponent* ASC)
{
	for (TSubclassOf<UGameplayAbility> AbilityClass : TraversalShiftAbilitiesOrdered)
	{
		if (ASC->TryActivateAbilityByClass(AbilityClass, true))
		{
			FGameplayAbilitySpec* Spec;

			Spec = ASC->FindAbilitySpecFromClass(AbilityClass);
			if (Spec && Spec->IsActive())
			{
				return true;
			}
		}
	}

	return false;
}

bool UAbilityTraversalComponent::TryNormalAttackTraversal(UAbilitySystemComponent* ASC)
{
	for (TSubclassOf<UGameplayAbility> AbilityClass : TraversalNormalAttackAbilitiesOrdered)
	{
		if (ASC->TryActivateAbilityByClass(AbilityClass, true))
		{
			FGameplayAbilitySpec* Spec;

			Spec = ASC->FindAbilitySpecFromClass(AbilityClass);
			if (Spec && Spec->IsActive())
			{
				return true;
			}
		}
	}

	return false;
}

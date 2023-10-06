// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTTask_UseAbility.h"

#include "Characters/AA_CharacterBase.h"
#include "AI/AA_AIController.h"
#include "AbilitySystem/Components/AA_AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/AA_GameplayAbility.h"

UBTTask_UseAbility::UBTTask_UseAbility()
	: GameplayAbilityTags()
	, bIsActivating(false)
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_UseAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AAA_CharacterBase* OwnerCharacter = Cast<AAA_CharacterBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (IsValid(OwnerCharacter))
	{
		TArray<FGameplayTag> GameplayTags;
		GameplayAbilityTags.GetGameplayTagArray(GameplayTags);
		if (GameplayTags.Num() > 0)
		{
			int32 TagIndex = FMath::RandRange(1, GameplayTags.Num()) - 1;
			FGameplayTagContainer ActiveTagContainer(GameplayTags[TagIndex]);

			if (OwnerCharacter->GetAbilitySystemComponent()->TryActivateAbilitiesByTag(ActiveTagContainer, true))
			{
				TArray<UAA_GameplayAbility*> ActiveAbilities;
				OwnerCharacter->GetActiveAbilitiesWithTags(ActiveTagContainer, ActiveAbilities);
				if (ActiveAbilities.Num() > 0)
				{
					ActiveAbilities[0]->OnGameplayAbilityEnded.AddUFunction(this, FName("OnAbilityEnded"));
					bIsActivating = true;
					Result = EBTNodeResult::InProgress;
				}
			}
		}
		
	}

	return Result;
}

void UBTTask_UseAbility::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bIsActivating)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTTask_UseAbility::OnAbilityEnded(UGameplayAbility* GameplayAbility)
{
	bIsActivating = false;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "BT/BTTask_PlayBossPattern.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/BossCharacter.h"

UBTTask_PlayBossPattern::UBTTask_PlayBossPattern()
{
	NodeName = TEXT("Play Boss Pattern");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_PlayBossPattern::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon)
	{
		return EBTNodeResult::Failed;
	}

	ABossCharacter* Boss = Cast<ABossCharacter>(AICon->GetPawn());
	if (!Boss)
	{
		return EBTNodeResult::Failed;
	}

	Boss->PlayPatternMontage(PatternIndex);

	if (!Boss->IsPatternPlaying())
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_PlayBossPattern::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	ABossCharacter* Boss = Cast<ABossCharacter>(AICon->GetPawn());
	if (!Boss)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (!Boss->IsPatternPlaying())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
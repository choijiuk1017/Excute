// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BossCharacter.h"

#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"

#include "UI/HPBar.h"
#include "Components/WidgetComponent.h"
#include "Actor/BossAOEIndicator.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

ABossCharacter::ABossCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add(TEXT("Boss"));
}

void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HPBarComponent)
	{
		HPBarComponent->SetVisibility(false);
	}
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && BossHPBarWidgetClass)
	{
		BossHPBarWidget = CreateWidget<UHPBar>(PC, BossHPBarWidgetClass);

		if (BossHPBarWidget)
		{
			BossHPBarWidget->AddToViewport(10);
			BossHPBarWidget->SetVisibility(ESlateVisibility::Hidden);
			UpdateBossHPBarPercent();
		}
	}

}

void ABossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateBossHPBarVisibility();

}

void ABossCharacter::PlayPatternMontage(int32 Index)
{

	if (bIsDead)
	{
		return;
	}

	if (bIsParried)
	{
		return;
	}

	if (!PatternMontages.IsValidIndex(Index))
	{
		return;
	}

	if (!GetMesh())
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	UAnimMontage* PatternMontage = PatternMontages[Index];
	if (!PatternMontage)
	{
		return;
	}

	CurrentPatternIndex = Index;
	bIsPatternPlaying = true;

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	AnimInstance->Montage_Play(PatternMontage);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ABossCharacter::OnPatternMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, PatternMontage);
}

void ABossCharacter::OnPatternMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsPatternPlaying = false;
	CurrentPatternIndex = -1;

	if (!bIsDead && !bIsParried)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

bool ABossCharacter::IsPatternPlaying() const
{
	return bIsPatternPlaying;
}
void ABossCharacter::UpdateBossHPBarVisibility()
{
	if (!BossHPBarWidget)
	{
		return;
	}

	if (bIsDead)
	{
		BossHPBarWidget->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player)
	{
		BossHPBarWidget->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	const float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());

	if (Distance <= BossHPBarVisibleDistance)
	{
		BossHPBarWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		BossHPBarWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABossCharacter::UpdateBossHPBarPercent()
{
	if (!BossHPBarWidget)
	{
		return;
	}

	if (EnemyStats.MaxHP <= 0)
	{
		return;
	}

	const float Percent =
		static_cast<float>(EnemyStats.CurrentHP) /
		static_cast<float>(EnemyStats.MaxHP);

	BossHPBarWidget->SetHPBarPercent(Percent);
}

void ABossCharacter::TakeDamage(float damage)
{

	const bool bWasDead = bIsDead;

	Super::TakeDamage(damage);

	if (!bHalfHPPatternTriggered && EnemyStats.MaxHP > 0)
	{
		const float HPPercent =
			static_cast<float>(EnemyStats.CurrentHP) /
			static_cast<float>(EnemyStats.MaxHP);

		if (HPPercent <= 0.5f)
		{
			bHalfHPPatternTriggered = true;

			if (AAIController* AICon = Cast<AAIController>(GetController()))
			{
				if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
				{
					BB->SetValueAsBool(TEXT("CanUseHalfHPPattern"), true);
				}
			}
		}
	}
	UpdateBossHPBarPercent();

	if (!bWasDead && bIsDead)
	{
		if (BossHPBarWidget)
		{
			BossHPBarWidget->RemoveFromParent();
			BossHPBarWidget = nullptr;
		}

		if (!bBossDeadWidgetShown && BossDeadWidgetClass)
		{
			APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (PC)
			{
				BossDeadWidget = CreateWidget<UUserWidget>(PC, BossDeadWidgetClass);

				if (BossDeadWidget)
				{
					BossDeadWidget->AddToViewport(20);
					bBossDeadWidgetShown = true;
				}
			}
		}
	}
}

void ABossCharacter::StartAOEIndicatorCharge()
{
	if (!AOEIndicatorClass)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FVector SpawnLocation = GetActorLocation();

	FHitResult Hit;
	const FVector TraceStart = GetActorLocation();
	const FVector TraceEnd = TraceStart - FVector(0.f, 0.f, 3000.f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params))
	{
		SpawnLocation = Hit.ImpactPoint;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentAOEIndicator = World->SpawnActor<ABossAOEIndicator>(
		AOEIndicatorClass,
		SpawnLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (CurrentAOEIndicator)
	{
		CurrentAOEIndicator->StartCharge(AOEChargeDuration, AOERadius);
	}
}

void ABossCharacter::ExecuteAOEAttack()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector AttackCenter =
		CurrentAOEIndicator ? CurrentAOEIndicator->GetActorLocation() : GetActorLocation();

	TArray<FOverlapResult> OverlapResults;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	const bool bHit = World->OverlapMultiByChannel(
		OverlapResults,
		AttackCenter,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(AOERadius),
		Params
	);

	if (bHit)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* HitActor = Result.GetActor();

			if (!HitActor || !HitActor->ActorHasTag(TEXT("Player")))
			{
				continue;
			}

			UGameplayStatics::ApplyDamage(
				HitActor,
				AOEDamage,
				GetController(),
				this,
				UDamageType::StaticClass()
			);
		}
	}

	if (CurrentAOEIndicator)
	{
		CurrentAOEIndicator->ForceComplete();
		CurrentAOEIndicator->Destroy();
		CurrentAOEIndicator = nullptr;
	}
}

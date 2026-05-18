// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BossCharacter.h"

#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"

#include "UI/HPBar.h"
#include "Components/WidgetComponent.h"


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

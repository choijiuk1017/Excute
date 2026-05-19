// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/EnemyCharacter.h"
#include "BossCharacter.generated.h"


class UUserWidget;
/**
 * 
 */
UCLASS()
class PIXELATE_PROJECT_API ABossCharacter : public AEnemyCharacter
{
	GENERATED_BODY()
	
public:
	ABossCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	TArray<UAnimMontage*> PatternMontages;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	int32 CurrentPatternIndex = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	bool bIsPatternPlaying = false;

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	void PlayPatternMontage(int32 Index);

	UFUNCTION()
	void OnPatternMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	bool IsPatternPlaying() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|UI")
	TSubclassOf<UHPBar> BossHPBarWidgetClass;

	UPROPERTY()
	UHPBar* BossHPBarWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|UI")
	float BossHPBarVisibleDistance = 1500.f;

	void UpdateBossHPBarVisibility();
	void UpdateBossHPBarPercent();

	virtual void TakeDamage(float damage) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|UI")
	TSubclassOf<UUserWidget> BossDeadWidgetClass;

	UPROPERTY()
	UUserWidget* BossDeadWidget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|UI")
	bool bBossDeadWidgetShown = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AOE")
	TSubclassOf<class ABossAOEIndicator> AOEIndicatorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AOE")
	float AOERadius = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AOE")
	float AOEDamage = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AOE")
	float AOEChargeDuration = 1.5f;

	UPROPERTY()
	ABossAOEIndicator* CurrentAOEIndicator = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Boss|AOE")
	void StartAOEIndicatorCharge();

	UFUNCTION(BlueprintCallable, Category = "Boss|AOE")
	void ExecuteAOEAttack();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	bool bHalfHPPatternTriggered = false;
};

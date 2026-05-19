// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossAOEIndicator.generated.h"

class UDecalComponent;


UCLASS()
class PIXELATE_PROJECT_API ABossAOEIndicator : public AActor
{
	GENERATED_BODY()
	
public:
	ABossAOEIndicator();

	virtual void Tick(float DeltaTime) override;

	void StartCharge(float InDuration, float InRadius);
	void ForceComplete();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UDecalComponent* DecalComponent;

	float ChargeDuration = 1.5f;
	float ElapsedTime = 0.f;
	float TargetRadius = 1500.f;

	bool bCharging = false;

};

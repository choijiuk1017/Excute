// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/BossAOEIndicator.h"


#include "Components/DecalComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ABossAOEIndicator::ABossAOEIndicator()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetupAttachment(Root);

	DecalComponent->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));

	DecalComponent->DecalSize = FVector(500.f, TargetRadius, TargetRadius);
	DecalComponent->SetWorldScale3D(FVector(1.f, 0.1f, 0.1f));
}

void ABossAOEIndicator::BeginPlay()
{
	Super::BeginPlay();
}

void ABossAOEIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bCharging || !DecalComponent)
	{
		return;
	}

	ElapsedTime += DeltaTime;

	const float Alpha = FMath::Clamp(ElapsedTime / ChargeDuration, 0.f, 1.f);

	const float CurrentScale = FMath::Lerp(0.1f, 1.0f, Alpha);

	DecalComponent->SetWorldScale3D(FVector(
		1.0f,
		CurrentScale,
		CurrentScale
	));
}

void ABossAOEIndicator::StartCharge(float InDuration, float InRadius)
{
	ChargeDuration = FMath::Max(0.01f, InDuration);
	TargetRadius = InRadius;
	ElapsedTime = 0.f;
	bCharging = true;

	if (DecalComponent)
	{
		DecalComponent->DecalSize = FVector(500.f, TargetRadius, TargetRadius);
		DecalComponent->SetWorldScale3D(FVector(1.f, 0.1f, 0.1f));
	}
}

void ABossAOEIndicator::ForceComplete()
{
	bCharging = false;

	if (DecalComponent)
	{
		DecalComponent->DecalSize = FVector(
			500.f,
			TargetRadius,
			TargetRadius
		);
	}
}
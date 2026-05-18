// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/NextLevelTrigger.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

ANextLevelTrigger::ANextLevelTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(Root);

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	TriggerBox->SetBoxExtent(FVector(100.f, 100.f, 100.f));

	NextLevelName = TEXT("Map02");
}

void ANextLevelTrigger::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(
		this,
		&ANextLevelTrigger::OnOverlapBegin
	);
}

void ANextLevelTrigger::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (bTriggered)
	{
		return;
	}

	if (!OtherActor)
	{
		return;
	}

	if (!OtherActor->ActorHasTag(TEXT("Player")))
	{
		return;
	}

	bTriggered = true;

	if (bUseOpenLevel)
	{
		UGameplayStatics::OpenLevel(
			this,
			NextLevelName
		);
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/CrossBossTrigger.h"

#include "Character/BossCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"

ACrossBossTrigger::ACrossBossTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	CrossMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrossMesh"));
	CrossMesh->SetupAttachment(Root);

	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(Root);
	InteractionBox->SetBoxExtent(FVector(120.f, 120.f, 160.f));

	InteractionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionBox->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionBox->SetGenerateOverlapEvents(true);
}

void ACrossBossTrigger::BeginPlay()
{
	Super::BeginPlay();

	InteractionBox->OnComponentBeginOverlap.AddDynamic(
		this,
		&ACrossBossTrigger::OnInteractionBeginOverlap
	);

	InteractionBox->OnComponentEndOverlap.AddDynamic(
		this,
		&ACrossBossTrigger::OnInteractionEndOverlap
	);
}

void ACrossBossTrigger::OnInteractionBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!OtherActor)
	{
		return;
	}

	if (!OtherActor->ActorHasTag(TEXT("Player")))
	{
		return;
	}

	CurrentPlayer = OtherActor;
}

void ACrossBossTrigger::OnInteractionEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	if (!OtherActor)
	{
		return;
	}

	if (OtherActor == CurrentPlayer)
	{
		CurrentPlayer = nullptr;
	}
}

bool ACrossBossTrigger::CanInteract(AActor* Interactor) const
{
	if (bBossSpawned)
	{
		return false;
	}

	if (!Interactor)
	{
		return false;
	}

	if (Interactor != CurrentPlayer)
	{
		return false;
	}

	if (!Interactor->ActorHasTag(TEXT("Player")))
	{
		return false;
	}

	if (!BossClass || !BossSpawnPoint)
	{
		return false;
	}

	return true;
}

void ACrossBossTrigger::Interact(AActor* Interactor)
{
	if (!CanInteract(Interactor))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FTransform SpawnTransform = BossSpawnPoint->GetActorTransform();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ABossCharacter* SpawnedBoss = World->SpawnActor<ABossCharacter>(
		BossClass,
		SpawnTransform,
		SpawnParams
	);

	if (!SpawnedBoss)
	{
		return;
	}

	bBossSpawned = true;

	SetAllCollisionToOverlap();

	if (bDestroyAfterSpawn)
	{
		Destroy();
	}
}

void ACrossBossTrigger::SetAllCollisionToOverlap()
{
	TArray<UPrimitiveComponent*> PrimitiveComponents;
	GetComponents<UPrimitiveComponent>(PrimitiveComponents);

	for (UPrimitiveComponent* Comp : PrimitiveComponents)
	{
		if (!Comp)
		{
			continue;
		}

		Comp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Comp->SetCollisionResponseToAllChannels(ECR_Overlap);
		Comp->SetGenerateOverlapEvents(true);
	}
}
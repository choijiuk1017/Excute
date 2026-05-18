// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrossBossTrigger.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class ABossCharacter;

UCLASS()
class PIXELATE_PROJECT_API ACrossBossTrigger : public AActor
{
	GENERATED_BODY()
public:
	ACrossBossTrigger();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CrossMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* InteractionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	TSubclassOf<ABossCharacter> BossClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	AActor* BossSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	bool bDestroyAfterSpawn = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss")
	bool bBossSpawned = false;

	UPROPERTY()
	AActor* CurrentPlayer = nullptr;

	UFUNCTION()
	void OnInteractionBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnInteractionEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
	void SetAllCollisionToOverlap();
public:
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Interactor);

	UFUNCTION(BlueprintPure, Category = "Interaction")
	bool CanInteract(AActor* Interactor) const;
};

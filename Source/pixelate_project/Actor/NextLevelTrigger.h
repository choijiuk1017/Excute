// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NextLevelTrigger.generated.h"

class UBoxComponent;

UCLASS()
class PIXELATE_PROJECT_API ANextLevelTrigger : public AActor
{
	GENERATED_BODY()
	
public:
	ANextLevelTrigger();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	FName NextLevelName;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	bool bUseOpenLevel = true;

	bool bTriggered = false;

};

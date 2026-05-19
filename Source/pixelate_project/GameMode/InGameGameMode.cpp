// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/InGameGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void AInGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		return;
	}

	PC->SetIgnoreMoveInput(false);
	PC->SetIgnoreLookInput(false);
	PC->bShowMouseCursor = false;

	FInputModeGameOnly InputMode;
	PC->SetInputMode(InputMode);
}
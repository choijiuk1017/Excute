// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TitlePlayerController.h"

#include "Blueprint/UserWidget.h"

ATitlePlayerController::ATitlePlayerController()
{
	bShowMouseCursor = true;
}

void ATitlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);

	if (TitleWidgetClass)
	{
		TitleWidget = CreateWidget<UUserWidget>(this, TitleWidgetClass);

		if (TitleWidget)
		{
			TitleWidget->AddToViewport();

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(TitleWidget->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			SetInputMode(InputMode);

			bShowMouseCursor = true;
		}
	}
}
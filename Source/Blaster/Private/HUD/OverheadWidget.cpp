// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerName(APawn* InPawn)
{
	const APlayerState* PS = InPawn->GetPlayerState<APlayerState>();
	const FString GetPlayerName = PS ? PS->GetPlayerName() : FString(TEXT("Player"));
	SetDisplayText(GetPlayerName);
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerController/BlasterPlayerController.h"
#include "HUD/CharacterOverlay.h"
#include "HUD/BlasterHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Character/BlasterCharacter.h"

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
}

void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn))
	{
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
		
	}
}

void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD && BlasterHUD->CharacterOverlay)
	{
		if (BlasterHUD->CharacterOverlay->HealthBar)
		{
			const float HealthPercent = Health / MaxHealth;
			BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		}

		if (BlasterHUD->CharacterOverlay->HealthText)
		{
			const FString HealthText = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
			BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
		}
	}
}

void ABlasterPlayerController::SetHUDScore(float Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD && BlasterHUD->CharacterOverlay)
	{
		if (BlasterHUD->CharacterOverlay->ScoreAmount)
		{
			const FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
			BlasterHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
		}
	}
}

void ABlasterPlayerController::SetHUDDefeats(int32 Defeats)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD && BlasterHUD->CharacterOverlay)
	{
		if (BlasterHUD->CharacterOverlay->DefeatsAmount)
		{
			const FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
			BlasterHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
		}
	}
}

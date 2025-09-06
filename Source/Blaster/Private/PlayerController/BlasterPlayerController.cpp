// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerController/BlasterPlayerController.h"

#include "BlasterComponents/CombatComponent.h"
#include "HUD/CharacterOverlay.h"
#include "HUD/BlasterHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Character/BlasterCharacter.h"
#include "GameFramework/GameMode.h"
#include "GameMode/BlasterGameMode.h"
#include "GameState/BlasterGameState.h"
#include "HUD/Announcement.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PlayerState/BlasterPlayerState.h"

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	ServerCheckMatchState();
}

void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn))
	{
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
		
	}
}

void ABlasterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SetHUDTime();
	CheckTimeSync(DeltaSeconds);
	PollInit();
}

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerController, MatchState);
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
	else
	{
		bInitializeCharacterOverlay = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
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
	else
	{
		bInitializeCharacterOverlay = true;
		HUDScore = Score;
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
	else
	{
		bInitializeCharacterOverlay = true;
		HUDDefeats = Defeats;
	}
}

void ABlasterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD && BlasterHUD->CharacterOverlay)
	{
		if (BlasterHUD->CharacterOverlay->WeaponAmmoAmount)
		{
			const FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
			BlasterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
		}
	}
}

void ABlasterPlayerController::SetHUDCarriedAmmo(int32 CarriedAmmo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD && BlasterHUD->CharacterOverlay)
	{
		if (BlasterHUD->CharacterOverlay->CarriedAmmoAmount)
		{
			const FString CarriedAmmoText = FString::Printf(TEXT("%d"), CarriedAmmo);
			BlasterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(CarriedAmmoText));
		}
	}
}

void ABlasterPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD && BlasterHUD->CharacterOverlay)
	{
		if (BlasterHUD->CharacterOverlay->MatchCountdownText)
		{
			if (CooldownTime < 0.f)
			{
				BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
				return;
			}
			const int32 Min = FMath::FloorToInt(CountdownTime / 60.f);
			const int32 Sec = CountdownTime - Min * 60;

			const FString CountdownText = FString::Printf(TEXT("%02d : %02d"), Min, Sec);
			BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
		}
	}
}

void ABlasterPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD && BlasterHUD->Announcement)
	{
		if (BlasterHUD->Announcement->WarmupTime)
		{
			if (CooldownTime < 0.f)
			{
				BlasterHUD->Announcement->WarmupTime->SetText(FText());
				return;
			}

			const int32 Min = FMath::FloorToInt(CountdownTime / 60.f);
			const int32 Sec = CountdownTime - Min * 60;

			const FString CountdownText = FString::Printf(TEXT("%02d : %02d"), Min, Sec);
			BlasterHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
		}
	}
}

void ABlasterPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::Cooldown) TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);	
		}
	}

	CountdownInt = SecondsLeft;
}

void ABlasterPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (BlasterHUD && BlasterHUD->CharacterOverlay)
		{
			CharacterOverlay = BlasterHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				SetHUDHealth(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDefeats(HUDDefeats);
			}
		}
	}
}

void ABlasterPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void ABlasterPlayerController::ServerCheckMatchState_Implementation()
{
	if (const ABlasterGameMode* GameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidGame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);
	}
}

void ABlasterPlayerController::ClientJoinMidGame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);

	if (BlasterHUD && MatchState == MatchState::WaitingToStart)
	{
		BlasterHUD->AddAnnouncement();
	}
}

void ABlasterPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ABlasterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float ABlasterPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		ServerRequestServerTime(GetServerTime());
	}
}

void ABlasterPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ABlasterPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();	
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ABlasterPlayerController::HandleMatchHasStarted()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay == nullptr) BlasterHUD->AddCharacterOverlay();
		if (BlasterHUD->Announcement)
		{
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);	
		}
	}
}

void ABlasterPlayerController::HandleCooldown()
{
	UE_LOG(LogTemp, Warning, TEXT("HandleCooldown"));
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleCooldown"));
		BlasterHUD->CharacterOverlay->RemoveFromParent();
		if (BlasterHUD->Announcement && BlasterHUD->Announcement->AnnouncementText && BlasterHUD->Announcement->InfoText)
		{
			UE_LOG(LogTemp, Warning, TEXT("HandleCooldown"));
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("New Match Starts In : ");
			BlasterHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			if (const ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this)))
			{
				const ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
				TArray<ABlasterPlayerState*> TopPlayers = BlasterGameState->TopScoringPlayers;
				FString InfoTextString;
				if (TopPlayers.Num() == 0)
				{
					InfoTextString = FString("There is no winner.");
				}
				else if (TopPlayers.Num() == 1 && TopPlayers[0] == BlasterPlayerState)
				{
					InfoTextString = FString("You are the winner!");
				}
				else if (TopPlayers.Num() == 1)
				{
					InfoTextString = FString::Printf(TEXT("Winner : \n%s"), *TopPlayers[0]->GetPlayerName());
				}
				else if (TopPlayers.Num() > 1)
				{
					InfoTextString = FString("Players tied for the win : \n");
					for (const auto TiedPlayer : TopPlayers)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
					}
				}

				BlasterHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}
			
		}
	}
	if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn()))
	{
		BlasterCharacter->bDisableGameplay = true;
		BlasterCharacter->GetCombat()->FireButtonPressed(false);
	}
}

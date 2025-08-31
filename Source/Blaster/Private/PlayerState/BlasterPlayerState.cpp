// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/BlasterPlayerState.h"

#include "Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "PlayerController/BlasterPlayerController.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerState, Defeats);
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		PlayerController = PlayerController == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : PlayerController;
		if (PlayerController)
		{
			PlayerController->SetHUDScore(GetScore());
		}
	}
}

void ABlasterPlayerState::OnRep_Defeats()
{
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		PlayerController = PlayerController == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : PlayerController;
		if (PlayerController)
		{
			PlayerController->SetHUDDefeats(Defeats);
		}
	}
}

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);

	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		PlayerController = PlayerController == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : PlayerController;
		if (PlayerController)
		{
			PlayerController->SetHUDScore(GetScore());
		}
	}
}

void ABlasterPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		PlayerController = PlayerController == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : PlayerController;
		if (PlayerController)
		{
			PlayerController->SetHUDDefeats(Defeats);
		}
	}
}
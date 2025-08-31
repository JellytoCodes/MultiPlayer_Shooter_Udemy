// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

class ABlasterPlayerController;
class ABlasterCharacter;

UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()

public :
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

#pragma region Replication Notifies
	virtual void OnRep_Score() override;

	UFUNCTION()
	virtual void OnRep_Defeats();
#pragma endregion

	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);

private :
	UPROPERTY()
	ABlasterCharacter* Character;

	UPROPERTY()
	ABlasterPlayerController* PlayerController;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;

	
};



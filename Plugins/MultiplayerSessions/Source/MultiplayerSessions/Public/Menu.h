// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

class UMultiplayerSessionsSubsystem;
class UButton;

UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public :
	UFUNCTION(BlueprintCallable)
	void MenuSetup(	int32 NumberOfPublicConnections = 4, 
					FString TypeOfMatch = TEXT("FreeForAll"), 
					FString LobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/Lobby")), 
					FString GamePath = FString(TEXT("/Game/ThirdPerson/Maps/StartMap")));

protected :
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	/**
	 * Callbacks for the custom delegates on the MultiplayerSessionsSubsystem
	 */
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

private :

	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	UButton* StartButton;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	UFUNCTION()
	void StartButtonClicked();

	void MenuTearDown();

	// The subsystem designed to handle all online session functionality
	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	int32 NumPublicConnections = 4;
	FString MatchType = TEXT("FreeForAll");
	FString PathToLobby = TEXT("");
	FString PathToGame = TEXT("");
};

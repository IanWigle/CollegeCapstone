// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"
#include "LobbyGameMode.h"
#include "MechGameInstance.h"
#include "GameFramework/PlayerState.h"

void ALobbyPlayerController::Server_PlayerIsReady_Implementation(int UserID, int currentChoice)
{
	UMechGameInstance* gameInstance = Cast<UMechGameInstance>(GetGameInstance());

	if (!gameInstance->m_PlayersChoices.Contains(UserID))
	{

		//Notify gamemode that a player is ready
		ALobbyGameMode* gameMode = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
		if (gameMode)
		{
			gameInstance->m_PlayersChoices.Add(UserID, gameInstance->m_VariousChoices[currentChoice]);
			gameMode->PlayerIsReady();
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ALobbyGameState::Server_PlayerIsReady_Implementation - Player successfully readied"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ALobbyGameState::Server_PlayerIsReady_Implementation - playersChoice already saved"));
	}
}

void ALobbyPlayerController::Ready(int currentChoice)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ALobbyGameState::Ready - NetPlayerIndex: ") + FString::FromInt(PlayerState->GetPlayerId()));
	Server_PlayerIsReady(PlayerState->GetPlayerId(), currentChoice);
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeUIOnly input;
	SetInputMode(input);
}

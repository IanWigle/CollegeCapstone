// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "MechGameInstance.h"
#include "LobbyGameState.h"
#include "LobbyPlayerController.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	m_NumOfActivePlayers--;

	if (m_NumOfActivePlayers == 0)
	{
		UMechGameInstance* gameInstance = Cast<UMechGameInstance>(GetGameInstance());

		if (gameInstance)
		{
			gameInstance->StopHosting();
		}
	}

	Super::Logout(Exiting);
}

void ALobbyGameMode::StartGame()
{
	UMechGameInstance* gameInstance = Cast<UMechGameInstance>(GetGameInstance());
	ALobbyGameState* gameState = GetGameState<ALobbyGameState>();

	if (gameInstance == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Failed : GameInstance is not valid");
		return;
	}

	if (gameState == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Failed : GameState is not valid");
		return;
	}

	gameInstance->StartSession();

	UWorld* world = GetWorld();
	if (world == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Failed : World is not valid");
		return;
	}

	bUseSeamlessTravel = true;

		gameState->NetMulticast_RemoveOptions();

	if (gameInstance->m_UseTestLevel == false)
		world->ServerTravel(m_GameMapReference + "?listen");
	else
		world->ServerTravel(m_TestMapReference + "?listen");
}

void ALobbyGameMode::PlayerIsReady()
{
	UMechGameInstance* gameInstance = Cast<UMechGameInstance>(GetGameInstance());
	m_NumOfPlayersReady++;

	if (m_NumOfPlayersReady >= gameInstance->m_MaxNumberOfPlayers)
	{
		GetWorldTimerManager().SetTimer(m_StartGameHandle, this, &ALobbyGameMode::StartGame, m_TimeBeforeGameStarts);
	}
}


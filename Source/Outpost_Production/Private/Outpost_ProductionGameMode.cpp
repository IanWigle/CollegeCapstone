// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Outpost_ProductionGameMode.h"
#include "Outpost_ProductionCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Outpost_Production/Public/WaveMaster.h"
#include "Kismet/GameplayStatics.h"
#include "MechGameInstance.h"
#include "Outpost_ProductionGameState.h"
#include "WaveMaster.h"
#include "MainBase.h"
#include "GameFramework/PlayerState.h"

AOutpost_ProductionGameMode::AOutpost_ProductionGameMode()
{
	// set default pawn class to our Blueprinted character
	/*static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}*/
}

void AOutpost_ProductionGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AOutpost_ProductionGameMode::Server_ReturnPlayersToMenu_Implementation()
{
	for (APlayerMech* playerMech : m_Players)
	{
		Cast<APlayerController>(playerMech->GetController())->ClientTravel(m_MainMenuReferance + "?listen", ETravelType::TRAVEL_Absolute);
	}
}

void AOutpost_ProductionGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AOutpost_ProductionGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
#if !UE_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("AOutpost_ProductionGameMode::HandleStartingNewPlayer_Implementation : New Player!"));

	//Change the pawn choice
	FMechVariant variant = Cast<UMechGameInstance>(GetGameInstance())->m_PlayersChoices[NewPlayer->PlayerState->PlayerId];
	UE_LOG(LogTemp, Warning, TEXT("Players Net Index : %d"), NewPlayer->PlayerState->PlayerId);

	DefaultPawnClass = variant.VarientMech;
	UE_LOG(LogTemp, Warning, TEXT("Variant Name : %s"), *variant.VariantName);
#endif

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	m_NumOfActivePlayers++;

	APlayerMech* player = Cast<APlayerMech>(NewPlayer->GetPawn());

	m_Players.Add(player);
	m_PlayersRepaired.Add(NewPlayer->PlayerState->GetPlayerId(), false);

	if (m_WaveMaster)
	{
		m_WaveMaster->AddPlayer(player);
	}
	if (m_MainObjectivePointer)
	{
		player->m_MainBase = m_MainObjectivePointer;
	}
}

void AOutpost_ProductionGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	m_NumOfActivePlayers--;

	APlayerMech* player = Cast<APlayerMech>(Exiting->GetPawn());
	m_WaveMaster->RemovePlayer(player);
}

void AOutpost_ProductionGameMode::SetWaveMaster(AWaveMaster* waveMaster)
{
	m_WaveMaster = waveMaster;
	UE_LOG(LogTemp, Warning, TEXT("AOutpost_ProductionGameMode::SetWaveMaster : Test"));

	AOutpost_ProductionGameState* gameState = GetGameState<AOutpost_ProductionGameState>();

	if (gameState)
		gameState->m_WaveMaster = m_WaveMaster;

	//The first time the wavemaster is setup we ensure it is populated with every player we currently have
	for (int i = 0; i < m_Players.Num(); i++)
	{
		m_WaveMaster->AddPlayer(m_Players[i]);
	}	
}

void AOutpost_ProductionGameMode::SetMainObjective(AMainBase* mainObjective)
{
	m_MainObjectivePointer = mainObjective;

	//The first time the wavemaster is setup we ensure it is populated with every player we currently have
	for (int i = 0; i < m_Players.Num(); i++)
	{
		m_Players[i]->m_MainBase = mainObjective;
	}
}

void AOutpost_ProductionGameMode::ReadyPlayer(int playerID)
{
	if (m_PlayersRepaired.Contains(playerID))
	{
		m_PlayersRepaired[playerID] = true; 

		int numberReady = 0;
		for (auto& player : m_PlayersRepaired)
		{
			if (player.Value == true) numberReady++;
			UE_LOG(LogTemp, Warning, TEXT("AOutpost_ProductionGameMode::ReadyPlayer : Player Readied"));
		}

		AOutpost_ProductionGameState* gameState = Cast<AOutpost_ProductionGameState>(GetGameState<AOutpost_ProductionGameState>());

		if (gameState)
		{
			if (numberReady == gameState->GetCurrentPlayers().Num())
			{
				GetWorld()->GetTimerManager().SetTimer(m_WaveMaster->m_NextPhaseTimer, m_WaveMaster, &AWaveMaster::StartNextWave, 10.0f, false);
				UE_LOG(LogTemp, Warning, TEXT("AOutpost_ProductionGameMode::ReadyPlayer : Wave set to start"));
			}
		}
	}
}

void AOutpost_ProductionGameMode::AddToPlayersKilled(int numOfKilled)
{
	m_NumberOfPlayersDead += numOfKilled;

	if (m_NumberOfPlayersDead == m_Players.Num())
	{
		GetGameState<AOutpost_ProductionGameState>()->m_DialogueMaster->Server_PlayDialogueForAll(DialogueEnum::AllPlayersKilled);

		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &AOutpost_ProductionGameMode::Server_ReturnPlayersToMenu, m_DelayTillReturnToMenu);
	}
}

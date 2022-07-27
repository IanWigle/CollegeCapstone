// Fill out your copyright notice in the Description page of Project Settings.

#include "DialogueMaster.h"
#include "Sound/SoundCue.h"
#include "UnrealNetwork.h"
#include "Components/AudioComponent.h"
#include "PlayerMech.h"
#include "Outpost_ProductionGameState.h"

// Sets default values
ADialogueMaster::ADialogueMaster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
}

void ADialogueMaster::Server_PlayDialogueForAll_Implementation(DialogueEnum choice, bool overrideAlreadyPlaying)
{ 
	FDialogue varient = m_DialogueOptions[choice].GetRandomVariant();

	TArray<APlayerMech*> players = Cast<AOutpost_ProductionGameState>(GetWorld()->GetGameState())->GetCurrentPlayers();
	for (auto player : players)
	{
		player->Client_AddDialogueToQue(varient);
	}
}

void ADialogueMaster::Server_PlayDialogueForClient_Implementation(DialogueEnum choice, class APlayerMech* player, bool overrideAlreadyPlaying = false)
{
	player->Client_AddDialogueToQue(m_DialogueOptions[choice].GetRandomVariant());
}

void ADialogueMaster::BeginPlay()
{
	Super::BeginPlay();

	AOutpost_ProductionGameState* gameState = Cast<AOutpost_ProductionGameState>(GetWorld()->GetGameState());
	gameState->m_DialogueMaster = this;
}

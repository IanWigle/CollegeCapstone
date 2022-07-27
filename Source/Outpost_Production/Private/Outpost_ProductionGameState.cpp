// Fill out your copyright notice in the Description page of Project Settings.


#include "Outpost_ProductionGameState.h"
#include "WaveMaster.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerMech.h"
#include "Turret.h"
#include "DialogueMaster.h"

#include "BaseEntityAttributeSet.h"
#include "BaseGameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilities/Public/GameplayAbilitySet.h"
#include "UnrealNetwork.h"

TArray<APlayerMech*> AOutpost_ProductionGameState::GetCurrentPlayers()
{
	if (m_WaveMaster)
	{
		return m_WaveMaster->GetPlayersArray();
	}

	return TArray<APlayerMech*>();
}

TArray<float> AOutpost_ProductionGameState::GetOtherPlayersHealth(APlayerMech * owningPlayer)
{
	TArray<APlayerMech*> players = GetCurrentPlayers();
	TArray<float> health;
	for (APlayerMech* actor : players)
	{
		health.Add(actor->GetAbilitySystemComponent()->GetSet<UBaseEntityAttributeSet>()->Getm_Health());
	}

	return health;
}

TArray<class ATurret*> AOutpost_ProductionGameState::GetAllTurrets()
{
	TArray<APlayerMech*> players = GetCurrentPlayers();
	TArray<ATurret*> playerTurrets;

	for (APlayerMech* actor : players)
	{
		playerTurrets.Add(actor->GetTurret());
	}

	return playerTurrets;
}

TArray<bool> AOutpost_ProductionGameState::AreOtherTurretsDeployed()
{
	return TArray<bool>();
}

TArray<FName> AOutpost_ProductionGameState::GetPlayerNames()
{
	TArray<APlayerMech*> players = GetCurrentPlayers();
	TArray<FName> names;

	for (APlayerMech* actor : players)
	{
		names.Add(*actor->GetName());
	}

	return names;
}

void AOutpost_ProductionGameState::BaseDestroyed()
{
	m_WaveMaster->BeginInfinite();
}

void AOutpost_ProductionGameState::BeginPlay()
{
	Super::BeginPlay();
}

void AOutpost_ProductionGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOutpost_ProductionGameState, m_WaveMaster);
}
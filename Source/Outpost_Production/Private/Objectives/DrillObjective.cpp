// Fill out your copyright notice in the Description page of Project Settings.


#include "DrillObjective.h"
#include "..\..\Public\Objectives\DrillObjective.h"
#include "PlayerMech.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WaveMaster.h"

ADrillObjective::ADrillObjective()
{
	//We are only calling the _implementation_ of this function, so it is not an actual RPC. This saves us having to do the actual RPC call.
	NetMulticast_DisableObjective_Implementation();
}

void ADrillObjective::Server_OnDeath_Implementation(AActor* sourceActor)
{
	SignalThatObjectiveWasNotMet();

	NetMulticast_DisableObjective();

	if (GetWorld()->GetTimerManager().IsTimerActive(m_ObjectiveTimerHandle))
		GetWorld()->GetTimerManager().ClearTimer(m_ObjectiveTimerHandle);

	Super::Server_OnDeath_Implementation(sourceActor);
}

void ADrillObjective::StartObjective(FVector newLocation)
{
	UE_LOG(LogTemp, Log, TEXT("%s : StartObjective - Starting Objective"), *GetName())
	NetMulticast_EnableObjective(newLocation);

	if (!HasAuthority())
		return;

	if (!GetWorld()->GetTimerManager().IsTimerActive(m_ObjectiveTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(m_ObjectiveTimerHandle, this, &ADrillObjective::RewardPlayers, m_ObjectiveTimeLength);
	}
}

void ADrillObjective::RewardPlayers()
{
	if (HasAuthority() && GetWorld()->GetTimerManager().IsTimerActive(m_ObjectiveTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(m_ObjectiveTimerHandle);
	}
	UE_LOG(LogTemp, Log, TEXT("%s : Server_RewardPlayers_Implementation - Objective Met, rewarding players"), *GetName());
		
	for (AActor* player : m_WaveMaster->m_ArrayOfPlayers)
	{
		Cast<APlayerMech>(player)->AddResources(m_RewardAmount);
		UE_LOG(LogTemp, Log, TEXT("%s : Awarding %d to %s"), *GetName(), m_RewardAmount, *player->GetName());
	}

	NetMulticast_DisableObjective();
}

void ADrillObjective::NetMulticast_EnableObjective_Implementation(FVector newLocation)
{
	//sets actor to appear in the game and enables their tick and collision
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	SetActorEnableCollision(true);
	GetCharacterMovement()->GravityScale = 1.0f;
	SetActorLocation(newLocation, false, nullptr, ETeleportType::TeleportPhysics);
	m_bIsDead = false;
}

void ADrillObjective::NetMulticast_DisableObjective_Implementation()
{
	//sets actor to not appear in game and disables their tick and collision
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
	GetCharacterMovement()->GravityScale = 0.0f;
	m_bIsDead = true;
}

bool ADrillObjective::IsObjectiveActive()
{
	return GetWorld()->GetTimerManager().IsTimerActive(m_ObjectiveTimerHandle) && m_bIsDead == false;
}

void ADrillObjective::BeginPlay()
{
	AWaveMaster* waveMaster = Cast<AWaveMaster>(GetOwner());

	if (waveMaster)
		m_WaveMaster = waveMaster;

	Super::BeginPlay();
}

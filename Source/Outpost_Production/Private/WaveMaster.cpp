// Fill out your copyright notice in	the Description page of Project Settings.

#include "WaveMaster.h"
#include "Engine/Engine.h"
#include "BaseEnemy.h"
#include "Rusher.h"
#include "Hunter.h"
#include "..\..\Public\Objectives\DrillObjective.h"
#include "Outpost_ProductionGameMode.h"
#include "MainBase.h"
#include "SpawnZoneActor.h"
#include "PlayerMech.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MechGameInstance.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Outpost_ProductionGameState.h"


AWaveMaster::AWaveMaster()
{
	m_CurrentPhase = -1;
	m_CurrentWave = -1;

	SetReplicates(true);
}

AWaveMaster::~AWaveMaster()
{
}

void AWaveMaster::CollectTargetsForEnemies()
{
	//if the main base is nullptr, it will get the base
	if (!m_MainBase)
	{
		AOutpost_ProductionGameMode* gameMode = Cast<AOutpost_ProductionGameMode>(GetWorld()->GetAuthGameMode());
		AMainBase* base = Cast<AMainBase>(gameMode->GetMainObjectiveObject());
		if (base)
			m_MainBase = base;
	}
}

void AWaveMaster::AddPlayer(APlayerMech* player)
{
	if (!m_ArrayOfPlayers.Contains(player))
	{
		m_ArrayOfPlayers.Add(player);
	}
}

void AWaveMaster::RemovePlayer(APlayerMech* player)
{
	if (m_ArrayOfPlayers.Contains(player))
	{
		m_ArrayOfPlayers.Remove(player);
	}
}

void AWaveMaster::StartNextWave()
{
	m_CurrentPhase = -1;
	m_CurrentWave++;
	Cast<AOutpost_ProductionGameState>(GetWorld()->GetGameState())->m_DialogueMaster->Server_PlayDialogueForAll(DialogueEnum::WaveStarting);

	if (m_CurrentWave < m_Waves.Num())
	{
		NextPhase();
	}
	else
	{
		
		ReachedWinCondition();
	}
}

void AWaveMaster::EndWave()
{
	//clears the active drill list
	m_ActiveDrillList.Empty();

	//Implement In-Between wave pause
	GetWorld()->GetTimerManager().SetTimer(
		m_NextWaveHandle, this, &AWaveMaster::StartNextWave, m_Waves[m_CurrentWave].m_DelayTillNextWave, false);

	UE_LOG(LogTemp, Warning, TEXT("WaveMaster : preparing next wave in %f seconds"), m_Waves[m_CurrentWave].m_DelayTillNextWave);

	//Respawn players
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("AWaveMaster::EndWave - m_ArrayOfPlayers.Num(): %d"), m_ArrayOfPlayers.Num());
		for (auto player : m_ArrayOfPlayers)
		{
			if (player->m_bIsDead)
			{
				player->ReturnToSpawn();
				UE_LOG(LogTemp, Warning, TEXT("AWaveMaster::EndWave - Player returned to spawn"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AWaveMaster::EndWave - Player wasn't dead"));
			}
		}
		Cast<AOutpost_ProductionGameMode>(GetWorld()->GetAuthGameMode())->m_NumberOfPlayersDead = 0;
	}

	Cast<AOutpost_ProductionGameState>(GetWorld()->GetGameState())->m_DialogueMaster->Server_PlayDialogueForAll(DialogueEnum::WaveEnding);
}

void AWaveMaster::CheckEndWave()
{
	if (!HasAuthority())
		return;
	// IF the current phase is MORE OR EQUAL to the current wave's size of phases
	// AND the available rusher EQUALS the max rusher leader TIMES the max rushers per leader MINUS 1
	// AND the rusher leader EQUALS our max rush leader MINUS 1
	// AND we are not in infinite wave.
	if (m_CurrentPhase >= m_Waves[m_CurrentWave].m_Phases.Num() - 1 &&
		m_AvailableRusher == STATIC_MAX_RUSHERS - 1 &&
		m_AvailableRanged == STATIC_MAX_RANGED - 1 &&
		m_InfiniteWaveIsEnabled == false
		/*&&check boss and ranged*/)
	{
		EndWave();
		UE_LOG(LogTemp, Warning, TEXT("WaveMaster : All enemies are dead."))
	}
}

void AWaveMaster::NextPhase()
{
	FPhase phaseToSpawn;

	//collects the base
	CollectTargetsForEnemies();

	if (m_InfiniteWaveIsEnabled)
	{
		phaseToSpawn = m_InfinitePhase;
	}
	else
	{
		m_CurrentPhase++;
		if (m_CurrentPhase < m_Waves[m_CurrentWave].m_Phases.Num())
		{
			phaseToSpawn = m_Waves[m_CurrentWave].m_Phases[m_CurrentPhase];
		}
	}


	if (phaseToSpawn.m_NumSecondaryObjectives > 0)
	{
		m_NumberOfDrillsInQue += phaseToSpawn.m_NumSecondaryObjectives;
		SpawnDrill();		

		if (m_LaunchedSecObjForFirstTime == false)
		{
			Cast<AOutpost_ProductionGameState>(GetWorld()->GetGameState())->m_DialogueMaster->Server_PlayDialogueForAll(DialogueEnum::SecondObjExplained);
			m_LaunchedSecObjForFirstTime = true;
		}
		else
		{
			Cast<AOutpost_ProductionGameState>(GetWorld()->GetGameState())->m_DialogueMaster->Server_PlayDialogueForAll(DialogueEnum::SecondObjSent);
		}
	}

	//Spawn new Rushers
	for (int i = 0; i < phaseToSpawn.m_NumRusherPacksToSpawn; i++)
	{
		// Choose which spawn zone to spawn in.
		int spawnZoneIndex = FMath::RandHelper(m_RusherSpawnZones.Num());
		ASpawnZoneActor* spawnZone = m_RusherSpawnZones[spawnZoneIndex];

		for (int j = 0; j < m_NumberOfRushersPerPacks; j++)
		{
			FVector spawnPosition;

			if (!spawnZone)
			{
				spawnPosition = GetActorLocation();
			}
			else
			{
				FVector originPos;
				FVector extentPos;

				spawnZone->GetActorBounds(false, originPos, extentPos);
				spawnPosition = UKismetMathLibrary::RandomPointInBoundingBox(originPos, extentPos);
			}

			ARusher* rusher = GetAvailableRusher();

			if (rusher)
			{
				rusher->SetEnemyReferences(this, m_ArrayOfPlayers, m_MainBase, m_ActiveDrillList);
				rusher->NetMulticast_EnableEnemy(spawnPosition);
				AAIController* controller = Cast<AAIController>(rusher->GetController());
				if (controller)
				{
					controller->GetBlackboardComponent()->SetValueAsBool("IsActive", true);
				}
			}
		}
	}


	//Spawn new Ranged
	for (int j = 0; j < phaseToSpawn.m_NumRangedToSpawn; j++)
	{
		//GetAvailableRanged()->EnableEnemy();
		FVector spawnPosition;
		FVector originPos;
		FVector extentPos;

		int randomeIndex = FMath::RandHelper(m_HunterSpawnZones.Num());
		m_HunterSpawnZones[randomeIndex]->GetActorBounds(false, originPos, extentPos);
		spawnPosition = UKismetMathLibrary::RandomPointInBoundingBox(originPos, extentPos);
		UE_LOG(LogTemp, Warning, TEXT("originPos: %s,extentPos: %s"), *originPos.ToString(), *extentPos.ToString());

		AHunter* newHunter = GetAvailableRanged();

		if (newHunter)
		{
			newHunter->SetActorLocation(spawnPosition);
			newHunter->SetEnemyReferences(this, m_ArrayOfPlayers, m_MainBase, m_ActiveDrillList);
			newHunter->NetMulticast_EnableEnemy(spawnPosition);
			AAIController* controller = Cast<AAIController>(newHunter->GetController());
			if (controller)
			{
				controller->GetBlackboardComponent()->SetValueAsBool("IsActive", true);
			}
		}
	}

	//Spawn new Boss
	for (int j = 0; j < phaseToSpawn.m_NumBossesToSpawn; j++)
	{
		//GetAvailableBoss()->EnableEnemy();
	}
	GetWorld()->GetTimerManager().SetTimer(m_NextPhaseTimer, this, &AWaveMaster::NextPhase, phaseToSpawn.m_DelayToNextPhase, false);
}

ARusher* AWaveMaster::GetAvailableRusher()
{
	if (m_AvailableRusher >= 0 && m_RusherPool[m_AvailableRusher])
	{
		ARusher* returnVal = m_RusherPool[m_AvailableRusher];
		m_RusherPool[m_AvailableRusher] = nullptr;
		m_AvailableRusher--;
		return returnVal;
	}

	UE_LOG(LogTemp, Error, TEXT("AWaveMaster::GetAvailableRusher - NO RUSHERS AVAILABLE"));
	return nullptr;
}

void AWaveMaster::ReturnRusher(ARusher* rusher)
{
	//Ensure we weren't passed a bad value and that our array isn't already full
	if (rusher != nullptr && m_AvailableRusher < (STATIC_MAX_RUSHERS - 1))
	{
		m_AvailableRusher++;
		m_RusherPool[m_AvailableRusher] = rusher;
		rusher->SetActorLocation(GetActorLocation());
	}

	if (HasAuthority())
	{
		CheckEndWave();
	}
}

AHunter* AWaveMaster::GetAvailableRanged()
{
	if (m_AvailableRanged > 0 && m_RangedPool[m_AvailableRanged] != nullptr)
	{
		AHunter* returnVal = m_RangedPool[m_AvailableRanged];
		m_RangedPool[m_AvailableRanged] = nullptr;
		m_AvailableRanged--;
		UE_LOG(LogTemp, Warning, TEXT("AWaveMaster::GetAvailableRanged - Successful"));
		return returnVal;
	}

	return nullptr;
}

void AWaveMaster::ReturnRanged(AHunter* ranged)
{
	if (ranged != nullptr && m_AvailableRanged < (STATIC_MAX_RANGED - 1))
	{
		m_AvailableRanged++;
		m_RangedPool[m_AvailableRanged] = ranged;
		ranged->SetActorLocation(GetActorLocation());
	}

	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("AWaveMaster::ReturnRanged"));
		CheckEndWave();
	}
}

AActor* AWaveMaster::GetAvailableBoss()
{
	return nullptr;
}

void AWaveMaster::ReturnBoss(AActor* boss)
{
	//Fill in at later date
}

ADrillObjective* AWaveMaster::GetAvailableDrill()
{
	if (m_AvailableDrills >= 0 && m_DrillPool[m_AvailableDrills])
	{
		ADrillObjective* returnVal = m_DrillPool[m_AvailableDrills];
		m_DrillPool[m_AvailableDrills] = nullptr;
		m_AvailableDrills--;
		return returnVal;
	}

	return nullptr;
}

void AWaveMaster::ReturnDrill(ADrillObjective* drill)
{
	//Ensure we weren't passed a bad value and that our array isn't already full
	if (drill != nullptr && m_AvailableDrills < (STATIC_MAX_DRILLS - 1))
	{
		m_AvailableDrills++;
		m_DrillPool[m_AvailableDrills] = drill;
		drill->NetMulticast_DisableObjective();

		m_ActiveDrillList.Remove(drill);
	}

	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("AWaveMaster::Returning Rusher %s"), *drill->GetName());
		CheckEndWave();
	}
}

void AWaveMaster::ResetEnemyReferences(ABaseEnemy* enemyToReset)
{
	//checks if it is not null, is valid
	if (enemyToReset)
		//gives the enemy its list of references
		enemyToReset->SetEnemyReferences(this, m_ArrayOfPlayers, m_MainBase, m_ActiveDrillList);
}

void AWaveMaster::SpawnDrill()
{
	int spawnZoneIndex = FMath::RandHelper(m_DrillSpawnZones.Num());
	ASpawnZoneActor* spawnZone = m_DrillSpawnZones[spawnZoneIndex];

	FVector spawnPosition;
	if (!spawnZone)
	{
		spawnPosition = GetActorLocation();
	}
	else
	{
		FVector originPos;
		FVector extentPos;

		spawnZone->GetActorBounds(false, originPos, extentPos);
		spawnPosition = UKismetMathLibrary::RandomPointInBoundingBox(originPos, extentPos);
	}

	ADrillObjective* drill = GetAvailableDrill();

	if (drill)
	{
		drill->StartObjective(spawnPosition);
		m_ActiveDrillList.Add(drill);

		UE_LOG(LogTemp, Log, TEXT("WaveMaster.SpawnDrill - Spawned %s"), *drill->GetName());
	}

	m_NumberOfDrillsInQue--;



	if (m_NumberOfDrillsInQue > 0)
		SpawnDrill();
}

void AWaveMaster::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
		return;

	AOutpost_ProductionGameMode* gameMode = GetWorld()->GetAuthGameMode<AOutpost_ProductionGameMode>();
	if (gameMode)
	{
		gameMode->SetWaveMaster(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AWaveMaster::BeginPlay - game mode cast failed!"));
	}

	//allows it to spawn actor if it would collide with other actors
	FActorSpawnParameters enemySpawnParams;
	enemySpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FActorSpawnParameters drillSpawnParams;
	drillSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	drillSpawnParams.Owner = Cast<AActor>(this);

	//TArray<AActor*> allEnemies;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseEnemy::StaticClass(), allEnemies);
	unsigned int currentRusher = 0;
	unsigned int currentHunter = 0;
	for (ABaseEnemy* enemy : m_EnemiesInLevel)
	{
		ARusher* rusher = Cast<ARusher>(enemy);
		if (rusher)
		{
			m_RusherPool[currentRusher] = rusher;
			m_RusherPool[currentRusher]->SetOwner(this);
			currentRusher++;
			continue;
		}

		AHunter* hunter = Cast<AHunter>(enemy);
		if (hunter)
		{
			m_RangedPool[currentHunter] = hunter;
			m_RangedPool[currentHunter]->SetOwner(this);
			currentHunter++;
			continue;
		}
	}


	for (int p = 0; p < STATIC_MAX_DRILLS; p++)
	{
		m_DrillPool[p] = Cast<ADrillObjective>(GetWorld()->SpawnActor<ADrillObjective>(m_DrillClass, GetActorLocation(), FRotator::ZeroRotator, drillSpawnParams));
	}
	
	m_AvailableRusher = STATIC_MAX_RUSHERS - 1;
	m_AvailableRanged = STATIC_MAX_RANGED - 1;
	m_AvailableDrills = STATIC_MAX_DRILLS - 1;
}

void AWaveMaster::ReachedWinCondition_Implementation()
{
	//Ensure we don't accidentally start another phase
	GetWorld()->GetTimerManager().ClearTimer(m_NextPhaseTimer);


	for (AActor* mech : m_ArrayOfPlayers)
	{
		Cast<APlayerMech>(mech)->NetMulticast_DisplayWin();
		UE_LOG(LogTemp, Warning, TEXT("AWaveMaster.ReachedWinCondition - multicast display"));
	}
	Cast<AOutpost_ProductionGameState>(GetWorld()->GetGameState())->m_DialogueMaster->Server_PlayDialogueForAll(DialogueEnum::GameWon);
}

void AWaveMaster::GameWon()
{
	ReachedWinCondition();
	UE_LOG(LogTemp, Warning, TEXT("AWaveMaster.EndGame"));
}

void AWaveMaster::BeginInfinite()
{
	if (m_InfiniteWaveIsEnabled == true)
		return;

	m_InfiniteWaveIsEnabled = true;

	UE_LOG(LogTemp, Warning, TEXT("Entering Infinite Wave"));

	GetWorld()->GetTimerManager().ClearTimer(m_NextPhaseTimer);
	GetWorld()->GetTimerManager().SetTimer(m_NextPhaseTimer, this, &AWaveMaster::NextPhase, 3.0f, true);
}

FString AWaveMaster::RemainingTimeTillNextWave()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(m_NextWaveHandle))
	{
		float Answer = GetWorld()->GetTimerManager().GetTimerRemaining(m_NextWaveHandle);
		
		return FString::SanitizeFloat(Answer);
	}
	else
	{
		return "In Wave";
	}
}

bool AWaveMaster::WaitingForNextWave()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(m_NextWaveHandle))
		return true;
	else
		return false;
}

void AWaveMaster::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWaveMaster, m_ArrayOfPlayers);
	DOREPLIFETIME(AWaveMaster, m_MainBase);
	DOREPLIFETIME(AWaveMaster, m_InfiniteWaveIsEnabled);
	DOREPLIFETIME(AWaveMaster, m_CurrentWave);
}
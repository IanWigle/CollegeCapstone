// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WaveMaster.generated.h"

USTRUCT(BlueprintType)
struct FPhase
{
	GENERATED_BODY()
public:

	FPhase(int rusher, int ranged, int bosses, int numSecondaryObjectives, float delay)
	{
		m_NumRusherPacksToSpawn = rusher;
		m_NumRangedToSpawn = ranged;
		m_NumBossesToSpawn = bosses;
		m_NumSecondaryObjectives = numSecondaryObjectives;
		m_DelayToNextPhase = delay;
	}

	FPhase()
	{
		m_NumRusherPacksToSpawn = 0;
		m_NumRangedToSpawn = 0; 
		m_NumBossesToSpawn = 0;
		m_NumSecondaryObjectives = 0;
		m_DelayToNextPhase = 1.0f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int m_NumRusherPacksToSpawn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int m_NumRangedToSpawn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int m_NumBossesToSpawn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int m_NumSecondaryObjectives;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_DelayToNextPhase;
};

USTRUCT(BlueprintType)
struct FWave
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FPhase> m_Phases;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_DelayTillNextWave;
};
/**
 * 
 */
UCLASS()
class OUTPOST_PRODUCTION_API AWaveMaster : public AActor
{

	GENERATED_BODY()

public:
	static const int STATIC_MAX_RUSHERS = 80;
	static const int STATIC_MAX_RANGED = 30;
	static const int STATIC_MAX_BOSS = 1;

	static const int STATIC_MAX_DRILLS = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves | List")
		TArray<FWave> m_Waves;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves | Entities")
		TSubclassOf<class ARusher> m_RusherClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves | Entities")
		TSubclassOf<class AHunter> m_HunterClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves | Entities")
		TSubclassOf<class ADrillObjective> m_DrillClass;
	FTimerHandle m_NextPhaseTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves | Spawn Zones")
		TArray<class ASpawnZoneActor*> m_RusherSpawnZones;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves | Spawn Zones")
		TArray<class ASpawnZoneActor*> m_HunterSpawnZones;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves | Spawn Zones")
		TArray<class ASpawnZoneActor*> m_DrillSpawnZones;

    UPROPERTY(Replicated)
        TArray<class APlayerMech*> m_ArrayOfPlayers;
    UPROPERTY(Replicated)
        class AMainBase* m_MainBase;
	UPROPERTY(EditInstanceOnly)
		TArray<class ABaseEnemy*> m_EnemiesInLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves | Spawning")
		int m_NumberOfRushersPerPacks = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Waves | Infinite")
		bool m_InfiniteWaveIsEnabled = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves | Infinite")
		FPhase m_InfinitePhase;

	FTimerHandle m_NextWaveHandle;

	int m_CurrentPhase;

	UPROPERTY(BlueprintReadOnly, Replicated)
		int m_CurrentWave;

	UPROPERTY(EditAnywhere)
		float m_TimeBeforeWavesStart = 10.0f;

private:
	class ARusher* m_RusherPool[STATIC_MAX_RUSHERS];
	int m_AvailableRusher = 0;
	class AHunter* m_RangedPool[STATIC_MAX_RANGED];
	int m_AvailableRanged = 0;
	AActor* m_BossPool[STATIC_MAX_BOSS];
	int m_AvailableBoss = 0;
	ADrillObjective* m_DrillPool[STATIC_MAX_DRILLS];
	int m_AvailableDrills = 0;

	bool m_LaunchedSecObjForFirstTime = false;
	

	int m_NumberOfDrillsInQue;
	FTimerHandle m_DrillSpawnQueHandle;
	float m_DelayTillNextDrill;
	TArray<ADrillObjective*> m_ActiveDrillList;


public:
	AWaveMaster();
	~AWaveMaster();

	void StartNextWave();
	void EndWave();
	void CheckEndWave();
	void NextPhase();
	
	UFUNCTION(BlueprintCallable)
		void CollectTargetsForEnemies();

	UFUNCTION(BlueprintCallable)
	void AddPlayer(APlayerMech* player);
	void RemovePlayer(APlayerMech* player);
	TArray<APlayerMech*> GetPlayersArray() { return m_ArrayOfPlayers; }

	//Returns a pointer to a Rusher removed from the pool, returns nullptr if no Rushers available
	ARusher* GetAvailableRusher();
	void ReturnRusher(ARusher* rusher);

	//Returns a pointer to a Ranged removed from the pool, returns nullptr if no Ranged available
	AHunter* GetAvailableRanged();
	void ReturnRanged(AHunter* ranged);

	//Returns a pointer to a Boss removed from the pool, returns nullptr if no Boss available
	AActor* GetAvailableBoss();
	void ReturnBoss(AActor* boss);

	//Returns a pointer to a Drill removed from the pool, returns nullptr if no Drill available
	ADrillObjective* GetAvailableDrill();
	void ReturnDrill(class ADrillObjective* drill);

    UFUNCTION(BlueprintCallable)
        void GameWon();

    UFUNCTION(Server, Reliable, WithValidation)
        void ReachedWinCondition();
        bool ReachedWinCondition_Validate() { return true; }

	void BeginInfinite();

	UFUNCTION(BlueprintCallable, BlueprintGetter)
		int GetCurrentWave() { return m_CurrentWave; }
	UFUNCTION(BlueprintCallable, BlueprintGetter)
		int GetCurrentPhase() { return m_CurrentPhase; }

	UFUNCTION(BlueprintCallable)
    AMainBase* GetBase() { return m_MainBase; }

	UFUNCTION(BlueprintCallable)
	void ResetEnemyReferences(class ABaseEnemy* enemyToReset);

	void SpawnDrill();

	UFUNCTION(BlueprintCallable)
		FString RemainingTimeTillNextWave();
		//void RemainingTimeTillNextWave(UPARAM(ref) float& floatAnswer, UPARAM(ref) int64& intAnswer, UPARAM(ref) FString& stringAnswer);

	UFUNCTION(BlueprintCallable)
		bool WaitingForNextWave();



protected:
	virtual void BeginPlay() override;

	void InfinitePhase();
};

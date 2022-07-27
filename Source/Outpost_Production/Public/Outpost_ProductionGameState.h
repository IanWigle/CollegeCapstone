// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Outpost_ProductionGameState.generated.h"

/**
 * 
 */
UCLASS()
class OUTPOST_PRODUCTION_API AOutpost_ProductionGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		class AWaveMaster* m_WaveMaster;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class ADialogueMaster* m_DialogueMaster;

	UFUNCTION(BlueprintCallable, Category = "Player Data")
		TArray<APlayerMech*> GetCurrentPlayers();

	UFUNCTION(BlueprintCallable, Category = "Player Data")
		TArray<float> GetOtherPlayersHealth(APlayerMech* owningPlayer);

	UFUNCTION(BlueprintCallable, Category = "Player Data")
		TArray<class ATurret*> GetAllTurrets();

	UFUNCTION(BlueprintCallable, Category = "Player Data")
		TArray<bool> AreOtherTurretsDeployed();

	UFUNCTION(BlueprintCallable, Category = "Player Data")
		TArray<FName> GetPlayerNames();

	UFUNCTION(BlueprintCallable, Category = "Player Data")
		int NumberOfPlayers() { return GetCurrentPlayers().Num(); }

public:

	void BaseDestroyed();

protected:

	void BeginPlay();
	
};

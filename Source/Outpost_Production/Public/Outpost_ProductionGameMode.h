// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Outpost_ProductionGameMode.generated.h"

UCLASS()
class AOutpost_ProductionGameMode : public AGameModeBase
{
	GENERATED_BODY()

		//Members
public:
	UPROPERTY(EditAnywhere, Category = Main_Objective)
		class AMainBase* m_MainObjectivePointer;

	int m_NumOfActivePlayers;

	UPROPERTY(BlueprintReadOnly)
		class AWaveMaster* m_WaveMaster;

	UPROPERTY(EditAnywhere)
		FString m_MainMenuReferance;

	int m_NumberOfPlayersDead;
protected:

private:
	TArray<class APlayerMech*> m_Players;
	TMap<int, bool> m_PlayersRepaired;

	UPROPERTY(EditAnywhere)
		float m_DelayTillReturnToMenu = 2.0f;


public:
	AOutpost_ProductionGameMode();

	AMainBase* GetMainObjectiveObject() { return m_MainObjectivePointer; }

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting);

	void SetWaveMaster(AWaveMaster* waveMaster);
	void SetMainObjective(AMainBase* mainObjective);

	void ReadyPlayer(int playerID);

	UFUNCTION(BlueprintCallable)
		void AddToPlayersKilled(int numOfKilled = 1);
	UFUNCTION(BlueprintCallable)
		int GetNumberOfKilledPlayers() {return m_NumberOfPlayersDead;}



protected:
	virtual void BeginPlay() override;

private:

	UFUNCTION(Server, WithValidation, Reliable)
		void Server_ReturnPlayersToMenu();
		bool Server_ReturnPlayersToMenu_Validate() { return true; };

};

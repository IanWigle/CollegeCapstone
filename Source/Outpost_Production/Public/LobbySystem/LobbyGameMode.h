// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class OUTPOST_PRODUCTION_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lobby")
		int m_NumOfActivePlayers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby")
		float m_TimeBeforeGameStarts;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby")
		FString m_GameMapReference;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby")
		FString m_TestMapReference;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lobby")
		FTimerHandle m_StartGameHandle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lobby")
		int m_NumOfPlayersReady = 0;
public:

	virtual void PostLogin(APlayerController* NewPlayer);

	virtual void Logout(AController* Exiting);

	void PlayerIsReady();

protected:

	void StartGame();
	
};

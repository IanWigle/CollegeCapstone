// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LobbyGameState.generated.h"

/**
 * 
 */
UCLASS()
class OUTPOST_PRODUCTION_API ALobbyGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		void RemoveOptionWidget();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void NetMulticast_RemoveOptions();
		bool NetMulticast_RemoveOptions_Validate() { return true; }
};

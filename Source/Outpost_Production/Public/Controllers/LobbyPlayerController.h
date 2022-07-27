// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class OUTPOST_PRODUCTION_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
		void Ready(int currentChoice);

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_PlayerIsReady(int UserNetIndex, int currentChoice);
		bool Server_PlayerIsReady_Validate(int UserNetIndex, int currentChoice) { return true; };
protected:
	void BeginPlay() override;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MenuPawn.generated.h"

UCLASS()
class OUTPOST_PRODUCTION_API AMenuPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMenuPawn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAudioComponent* m_MusicPlayer;
};
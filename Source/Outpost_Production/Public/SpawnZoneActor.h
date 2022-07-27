// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnZoneActor.generated.h"

UCLASS()
class OUTPOST_PRODUCTION_API ASpawnZoneActor : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* m_SpawnVolume;

public:	
	// Sets default values for this actor's properties
	ASpawnZoneActor();

};

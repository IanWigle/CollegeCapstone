// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnZoneActor.h"
#include "Components/BoxComponent.h"

// Sets default values
ASpawnZoneActor::ASpawnZoneActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	m_SpawnVolume = CreateDefaultSubobject<UBoxComponent>("Spawn Volume");
	RootComponent = m_SpawnVolume;
}

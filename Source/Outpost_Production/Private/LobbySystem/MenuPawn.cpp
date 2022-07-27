// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuPawn.h"
#include "Components/AudioComponent.h"

// Sets default values
AMenuPawn::AMenuPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_MusicPlayer = CreateDefaultSubobject<UAudioComponent>("Music Player");
}

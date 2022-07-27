// Fill out your copyright notice in the Description page of Project Settings.


#include "Hunter_AIController.h"

void AHunter_AIController::BeginPlay()
{
	Super::BeginPlay();
}

void AHunter_AIController::RunBehavior()
{
	RunBehaviorTree(m_HunterBehaviortree);
}
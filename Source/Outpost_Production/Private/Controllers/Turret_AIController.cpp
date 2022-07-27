// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Turret.h"
#include "Engine/World.h"

void ATurret_AIController::BeginPlay()
{
	Super::BeginPlay();


	FTimerHandle handle;
	RunBehaviorTree(m_TurretBehaviorTree);
	auto turret = Cast<ATurret>(GetPawn());
	GetBlackboardComponent()->SetValueAsFloat("Cooldown", m_TurretBlueprint.GetDefaultObject()->m_AttackCooldown);
	UpdateBehaviorTreeValues(GetPawn());
}

//void ATurret_AIController::RunBehavior()
//{
//	RunBehaviorTree(m_TurretBehaviorTree);
//	auto turret = Cast<ATurret>(GetPawn());
//	GetBlackboardComponent()->SetValueAsFloat("Cooldown", m_TurretBlueprint.GetDefaultObject()->m_AttackCooldown);
//	UE_LOG(LogTemp, Warning, TEXT("ATurret_AIController.BeginPlay - Cooldown applied"));
//}
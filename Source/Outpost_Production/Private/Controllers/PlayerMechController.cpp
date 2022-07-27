// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMechController.h"
#include "PlayerMech.h"
#include "Engine/Engine.h"
#include "AbilitySystemComponent.h"

void APlayerMechController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Movement Controls
	InputComponent->BindAxis("MoveForward", this, &APlayerMechController::MoveUp);
	InputComponent->BindAxis("MoveRight", this, &APlayerMechController::MoveRight);

	// Camera
	InputComponent->BindAxis("Turn", this, &APlayerMechController::Turn);
	InputComponent->BindAxis("TurnAtRate", this, &APlayerMechController::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APlayerMechController::LookUp);
	InputComponent->BindAxis("LookUpRate", this, &APlayerMechController::LookUpRate);

	// Weapons/Turret
	InputComponent->BindAction("Interact", IE_Pressed, this, &APlayerMechController::Interact);
	InputComponent->BindAction("Turret", IE_Pressed, this, &APlayerMechController::Turret);
	InputComponent->BindAction("Reload", IE_Pressed, this, &APlayerMechController::Reload);
	InputComponent->BindAction("OpenMenu", IE_Pressed, this, &APlayerMechController::OpenMenu);

	FInputModeGameOnly inputMode;
	SetInputMode(inputMode);
}


void APlayerMechController::Interact()
{
	APlayerMech* const playerMech = Cast<APlayerMech>(GetPawn());
	if (playerMech)
	{
		//playerMech->SwitchWeapon(1,1);
		playerMech->Interact();
	}
}

void APlayerMechController::Turret()
{
	APlayerMech* const playerMech = Cast<APlayerMech>(GetPawn());
	if (playerMech)
	{
		playerMech->Server_BuildTurret();
	}
}

void APlayerMechController::Reload()
{
	APlayerMech* const playerMech = Cast<APlayerMech>(GetPawn());
	if (playerMech)
	{
		playerMech->Server_Reload();
	}
}

void APlayerMechController::OpenMenu()
{
	APlayerMech* const playerMech = Cast<APlayerMech>(GetPawn());
	if (playerMech)
	{
		playerMech->OpenPauseMenu();
	}
}

void APlayerMechController::MoveUp(float Value)
{
	APlayerMech* const playerMech = Cast<APlayerMech>(GetPawn());
	if (playerMech)
	{
		playerMech->MoveUp(Value);
	}
	
}

void APlayerMechController::MoveRight(float Value)
{
	APlayerMech* const playerMech = Cast<APlayerMech>(GetPawn());
	if (playerMech)
	{
		playerMech->MoveRight(Value);
	}
}

void APlayerMechController::Turn(float Rate)
{
    APlayerMech* const playerMech = Cast<APlayerMech>(GetPawn());

	Rate = (m_InverseControlls == true) ? (Rate * (m_MouseSensitivity)) * -1.0f : Rate * (m_MouseSensitivity);

    if (playerMech)
    {
        GetPawn()->AddControllerYawInput(Rate);
    }
}

void APlayerMechController::TurnAtRate(float Rate)
{
	APlayerMech* const playerMech = Cast<APlayerMech>(GetPawn());

	Rate = (m_InverseControlls == true) ? (Rate * (m_MouseSensitivity)) * -1.0f : Rate * (m_MouseSensitivity);

	if (playerMech)
	{
		playerMech->TurnAtRate(Rate);
	}
}

void APlayerMechController::LookUp(float Rate)
{
    APlayerMech* const playerMech = Cast<APlayerMech>(GetPawn());

	Rate = (m_InverseControlls == true) ? (Rate * (m_MouseSensitivity)) * -1.0f : Rate * (m_MouseSensitivity);

    if (playerMech)
    {
        GetPawn()->AddControllerPitchInput(Rate);
    }
}

void APlayerMechController::LookUpRate(float Rate)
{
	APlayerMech* const playerMech = Cast<APlayerMech>(GetPawn());

	Rate = (m_InverseControlls == true) ? (Rate * (m_MouseSensitivity)) * -1.0f : Rate * (m_MouseSensitivity);

	if (playerMech)
	{
		playerMech->LookUpRate(Rate);
	}
}

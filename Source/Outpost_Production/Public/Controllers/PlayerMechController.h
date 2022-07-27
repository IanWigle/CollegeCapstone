// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerMechController.generated.h"

/**
 * 
 */
UCLASS()
class OUTPOST_PRODUCTION_API APlayerMechController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void SetupInputComponent();

protected:
	void Interact();
	void Turret();
	void Reload();
	void OpenMenu();
	void MoveUp(float Value);
	void MoveRight(float Value);
	void Turn(float Rate);
	void TurnAtRate(float Rate);
	void LookUp(float Rate);
	void LookUpRate(float Rate);
	//void TurnTorsoRate(float Rate);
	//void TurnLegsRate(float Rate);

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_MouseSensitivity = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool m_InverseControlls = false;
};

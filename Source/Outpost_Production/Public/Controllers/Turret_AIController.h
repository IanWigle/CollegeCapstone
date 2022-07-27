// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Turret_AIController.generated.h"

/**
 * 
 */
UCLASS()
class OUTPOST_PRODUCTION_API ATurret_AIController : public AAIController
{
	GENERATED_BODY()

		//Members
public:
	UPROPERTY(EditAnywhere)
		UBehaviorTree* m_TurretBehaviorTree;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class ATurret> m_TurretBlueprint;

	UPROPERTY(BlueprintReadWrite)
		class ATurret* m_TurretReferance;

	//Methods
public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
		void UpdateBehaviorTreeValues(APawn* turret);
};

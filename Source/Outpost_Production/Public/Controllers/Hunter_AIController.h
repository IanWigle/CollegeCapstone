// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Hunter_AIController.generated.h"

/**
 *
 */
UCLASS()
class OUTPOST_PRODUCTION_API AHunter_AIController : public AAIController
{
	GENERATED_BODY()

		//Members
public:
	UPROPERTY(EditAnywhere)
		UBehaviorTree* m_HunterBehaviortree;


	//Methods
public:
	virtual void BeginPlay() override;

	virtual void RunBehavior();

	UFUNCTION(BlueprintImplementableEvent)
		void HunterDeath();
};

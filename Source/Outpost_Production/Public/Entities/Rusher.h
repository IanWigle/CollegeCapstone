// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "Rusher.generated.h"

/**
 *
 */
UCLASS()
class OUTPOST_PRODUCTION_API ARusher : public ABaseEnemy
{
	GENERATED_BODY()

	//Members
public:
	UPROPERTY(EditAnywhere)
		class UBehaviorTree* m_RusherBehaviourTree;


	//Methods
public:
	ARusher();

	void SetEnemyReferences(AWaveMaster* waveMaster, TArray<APlayerMech*> playerArray, AMainBase* base, TArray<ADrillObjective*> activeDrillList) override;

	//Function to Attack the target when in combat
	void Attack() override;

	void Server_OnDeath_Implementation(AActor* sourceActor) override;

};

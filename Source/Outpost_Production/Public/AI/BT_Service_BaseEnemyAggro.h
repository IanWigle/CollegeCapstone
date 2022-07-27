// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BT_Service_BaseEnemyAggro.generated.h"

/**
 * 
 */
UCLASS()
class OUTPOST_PRODUCTION_API UBT_Service_BaseEnemyAggro : public UBTService
{
	GENERATED_BODY()
	
protected:
	void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};

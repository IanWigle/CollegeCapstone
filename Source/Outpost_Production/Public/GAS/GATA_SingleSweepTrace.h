// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor_Trace.h"
#include "GATA_SingleSweepTrace.generated.h"

/**
 * 
 */
UCLASS()
class OUTPOST_PRODUCTION_API AGATA_SingleSweepTrace : public AGameplayAbilityTargetActor_Trace
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, config, meta = (ExposeOnSpawn = true), Category = Trace)
        float SphereRadius;

protected:
    virtual FHitResult PerformTrace(AActor* InSourceActor);
};

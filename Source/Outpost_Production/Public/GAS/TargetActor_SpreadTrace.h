// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "Abilities/GameplayAbilityTargetActor_Trace.h"
#include "TargetActor_SpreadTrace.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class OUTPOST_PRODUCTION_API ATargetActor_SpreadTrace : public AGameplayAbilityTargetActor_Trace
{
	GENERATED_BODY()
	
protected:
	virtual FHitResult PerformTrace(AActor* InSourceActor) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FunctionHelperLibrary.generated.h"

/**
 * 
 */
UCLASS()
class OUTPOST_PRODUCTION_API UFunctionHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
		static AActor* GetClosestObjectToSourceByFilter(UWorld* world, AActor* sourceObj, TSubclassOf<AActor> filter);

	UFUNCTION(BlueprintCallable)
		static AActor* GetClosestObjectToSourceByTag(UWorld* world, AActor* sourceObj, FName tag);

	UFUNCTION(BlueprintCallable)
		static AActor* GetClosestObjectToSourceByFilterPlusTag(UWorld* world, AActor* sourceObj, TSubclassOf<AActor> filter, FName tag);

	/*UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
		static void NetMulticast_DisableEntity(class ABaseEntity* entity);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
		static void NetMulticast_EnableEntity(ABaseEntity* entity, FVector newLocation);*/
};

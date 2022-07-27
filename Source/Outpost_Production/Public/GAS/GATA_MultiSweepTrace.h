// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "CollisionQueryParams.h"
#include "Engine/CollisionProfile.h"
#include "WorldCollision.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/GameplayAbilityTargetDataFilter.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GATA_MultiSweepTrace.generated.h"

/**
 *
 */
UCLASS(Blueprintable, notplaceable, config = Game)
class OUTPOST_PRODUCTION_API AGATA_MultiSweepTrace : public AGameplayAbilityTargetActor
{
	GENERATED_UCLASS_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Trace)
		float MaxRange;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Trace)
		float SphereRadius;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, config, meta = (ExposeOnSpawn = true), Category = Trace)
		FCollisionProfileName TraceProfile;

	// Does the trace affect the aiming pitch
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Trace)
		bool bTraceAffectsAimPitch;


	virtual TArray<FHitResult> PerformTrace(AActor* InSourceActor);

	void AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, FVector& OutTraceEnd, bool bIgnorePitch = false) const;

	static void MultiSweepTraceWithFilter(TArray<FHitResult>& OutHitResult, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params);

	FGameplayAbilityTargetDataHandle MakeTargetData(const TWeakObjectPtr<UGameplayAbility> InAbility, const TArray<FHitResult>& InResult) const;

	static bool ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& ClippedPosition);

	virtual void StartTargeting(UGameplayAbility* InAbility) override;
	virtual void ConfirmTargetingAndContinue() override;
};

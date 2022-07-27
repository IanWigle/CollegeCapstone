// Fill out your copyright notice in the Description page of Project Settings.


#include "GATA_MultiSweepTrace.h"

AGATA_MultiSweepTrace::AGATA_MultiSweepTrace(const FObjectInitializer& objInit)
{

}

TArray<FHitResult> AGATA_MultiSweepTrace::PerformTrace(AActor* InSourceActor)
{
	return TArray<FHitResult>();
}

void AGATA_MultiSweepTrace::AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, FVector& OutTraceEnd, bool bIgnorePitch) const
{
}

void AGATA_MultiSweepTrace::MultiSweepTraceWithFilter(TArray<FHitResult>& OutHitResult, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params)
{
}

FGameplayAbilityTargetDataHandle AGATA_MultiSweepTrace::MakeTargetData(const TWeakObjectPtr<UGameplayAbility> InAbility, const TArray<FHitResult>& InResult) const
{
	return FGameplayAbilityTargetDataHandle();
}

bool AGATA_MultiSweepTrace::ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& ClippedPosition)
{
	return false;
}

void AGATA_MultiSweepTrace::StartTargeting(UGameplayAbility* InAbility)
{
}

void AGATA_MultiSweepTrace::ConfirmTargetingAndContinue()
{
}

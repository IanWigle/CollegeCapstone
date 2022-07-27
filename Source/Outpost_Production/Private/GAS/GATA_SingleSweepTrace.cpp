// Fill out your copyright notice in the Description page of Project Settings.


#include "GATA_SingleSweepTrace.h"
#include "DrawDebugHelpers.h"

FHitResult AGATA_SingleSweepTrace::PerformTrace(AActor* InSourceActor)
{
    bool bTraceComplex = false;
    TArray<AActor*> ActorsToIgnore;

    ActorsToIgnore.Add(InSourceActor);

    FCollisionQueryParams Params(SCENE_QUERY_STAT(AGameplayAbilityTargetActor_SingleLineTrace), bTraceComplex);
    Params.bReturnPhysicalMaterial = true;
    Params.AddIgnoredActors(ActorsToIgnore);

    FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();// InSourceActor->GetActorLocation();
    FVector TraceEnd;
    FQuat TraceRot = StartLocation.GetTargetingTransform().GetRotation();
    AimWithPlayerController(InSourceActor, Params, TraceStart, TraceEnd);		//Effective on server and launching client only

    FCollisionShape TraceShape = FCollisionShape::MakeSphere(SphereRadius);

    // ------------------------------------------------------

    FHitResult ReturnHitResult;
    SweepWithFilter(ReturnHitResult, InSourceActor->GetWorld(), Filter, TraceStart, TraceEnd, TraceRot, TraceShape, TraceProfile.Name, Params);
    //Default to end of trace line if we don't hit anything.
    if (!ReturnHitResult.bBlockingHit)
    {
        ReturnHitResult.Location = TraceEnd;
    }
    if (AGameplayAbilityWorldReticle * LocalReticleActor = ReticleActor.Get())
    {
        const bool bHitActor = (ReturnHitResult.bBlockingHit && (ReturnHitResult.GetActor() != NULL));
        const FVector ReticleLocation = (bHitActor && LocalReticleActor->bSnapToTargetedActor) ? ReturnHitResult.GetActor()->GetActorLocation() : ReturnHitResult.Location;

        LocalReticleActor->SetActorLocation(ReticleLocation);
        LocalReticleActor->SetIsTargetAnActor(bHitActor);
    }

#if ENABLE_DRAW_DEBUG
    if (bDebug)
    {
        DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green);
        DrawDebugSphere(GetWorld(), TraceEnd, 100.0f, 16, FColor::Green);
    }
#endif // ENABLE_DRAW_DEBUG
    return ReturnHitResult;
}
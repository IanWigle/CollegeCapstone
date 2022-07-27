// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BaseFloraClass.generated.h"

UCLASS()
class OUTPOST_PRODUCTION_API ABaseFloraClass : public AActor
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USphereComponent* m_ProximitySphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USkeletalMeshComponent* m_FloraMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<AActor*> m_EnteredActors;

public:
	// Sets default values for this pawn's properties
	ABaseFloraClass();

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	UFUNCTION()
		void EndOverLap(UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent)
		void ActorEntersProximity(AActor* actor);

	UFUNCTION(BlueprintImplementableEvent)
		void ActorLeavesProximity(AActor* actor);
};

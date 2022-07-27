// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/BaseEntity.h"
#include "DrillObjective.generated.h"

/**
 * 
 */
UCLASS()
class OUTPOST_PRODUCTION_API ADrillObjective : public ABaseEntity
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
		int m_RewardAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
		float m_ObjectiveTimeLength;
protected:
	FTimerHandle m_ObjectiveTimerHandle;


public:
	ADrillObjective();

	virtual void Server_OnDeath_Implementation(AActor* sourceActor) override;
	void StartObjective(FVector newLocation);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
		void NetMulticast_EnableObjective(FVector newLocation);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
		void NetMulticast_DisableObjective();
	UFUNCTION(BlueprintImplementableEvent)
		void SignalThatObjectiveWasMet();
	UFUNCTION(BlueprintImplementableEvent)
		void SignalThatObjectiveHasStarted();
	UFUNCTION(BlueprintImplementableEvent)
		void SignalThatObjectiveWasNotMet();
	UFUNCTION(BlueprintCallable)
		bool IsObjectiveActive();

protected:
	virtual void BeginPlay() override;
	void RewardPlayers();
private:
	class AWaveMaster* m_WaveMaster;
};

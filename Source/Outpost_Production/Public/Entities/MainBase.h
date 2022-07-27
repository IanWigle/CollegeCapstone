// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "MainBase.generated.h"

UCLASS()
class OUTPOST_PRODUCTION_API AMainBase : public AInteractable
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TSubclassOf<class UUserWidget> m_UpgradeMenu;

    //AI VARIABLES------------------------------------------
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Point Distance")
        float m_TargetPointDistanceFromBase = 400.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI | Vector Point Array")
        TArray<FVector> m_PositionsForRushers;

    UPROPERTY(EditAnywhere, Category = "Visual | DamageStates")
        TArray<class USkeletalMesh*> m_BaseDamageStatesArray;
    UPROPERTY(VisibleAnywhere)
        int m_CurrentDSIndex;
	UPROPERTY(EditAnywhere, Category = "Regen Ability")
		float m_CooldownWhenAttacked = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Regen Ability")
		float m_TimeBetweenRegenTick = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Regen Ability")
		TSubclassOf<class UGameplayEffect> RegenEffect;

protected:

	FTimerHandle HealWaitHandle;
	FTimerHandle HealHandle;

    float m_CurrentHealthSectionMax;

public:
    // Sets default values for this actor's properties
    AMainBase();
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    virtual void Server_OnDeath_Implementation(AActor* sourceActor) override;

    UFUNCTION(BlueprintCallable)
        void StartUpgrade(class APlayerMech* player);

    UFUNCTION(BlueprintCallable)
        TArray<FVector> GetTargetPoints();

    virtual void Server_OnDamageTaken_Implementation(float damageAmount, ABaseEntity* damageOwner) override;

    UFUNCTION(NetMulticast, Reliable)
        virtual void NetMulticast_ChangeDamageState();

	UFUNCTION(BlueprintCallable)
		bool IsHealthRegenRunning();

	UFUNCTION(BlueprintCallable)
		bool IsWaitingToStartHeal();

	UFUNCTION(BlueprintCallable)
		float GetRemainingHealthLockTime();

	UFUNCTION(BlueprintCallable)
		float GetRemainingTImeBetweenHeal();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintImplementableEvent)
        void MakeUpgradeMenu(APlayerMech* player);

	void StopRegen();
	void ResetHealWait();
	void StartRegen();
	void RegenHealth();
};

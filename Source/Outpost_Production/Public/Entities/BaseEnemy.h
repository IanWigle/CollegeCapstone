// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseEntity.h"
#include "Abilities/GameplayAbility.h"
#include "BaseEnemy.generated.h"


USTRUCT(BlueprintType)
struct FTarget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		ABaseEntity* TargetEntity = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float AggroValue = 1.0f;
};

UCLASS()
class OUTPOST_PRODUCTION_API ABaseEnemy : public ABaseEntity
{
	GENERATED_BODY()

		//Members
protected:
	//Pointers
	UPROPERTY(EditAnywhere, Replicated, Category = "Combat | Target")
		AActor* m_CurrentTarget = nullptr;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Combat | Player Array")
		TArray<class APlayerMech*> m_ArrayOfPlayers;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Combat | Base")
		class AMainBase* m_MainBase = nullptr;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Combat | Secondary Objectives")
		TArray<class ADrillObjective*> m_ActiveDrillList;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Combat | Aggro")
		TArray<FTarget> m_TargetValues;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "WaveMaster | Assigned WM")
		class AWaveMaster* m_WaveMaster;

	//Audio
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio | Component")
		class UAudioComponent* m_AudioComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Audio | Sound")
		class USoundBase* m_SpawnSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Audio | Sound")
		TArray<USoundBase*> m_AttackSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Audio | Sound")
		TArray<USoundBase*> m_DeathSound;

    UPROPERTY(BlueprintReadOnly)
        bool m_IsActive;

    UPROPERTY(BlueprintReadWrite, Category = "Abilites | Enable Attack")
        bool m_CanAttack = true;

    UPROPERTY(BlueprintReadOnly, Category = "Abilites | Attack Cooldown")
        FTimerHandle m_AttackCooldown;

	UPROPERTY(EditAnywhere, Category = "Attack")
		TSubclassOf<class UBaseGameplayAbility> EnemyAttack;

	//Members changed per enemy
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat | Resources")
		int m_ResourcesWorth = 5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat | Aggro")
		float  m_MainBaseAggroWeight = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat | Aggro")
		float m_PlayerMechAggroWeight = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat | Aggro")
		float m_TurretAggroWeight = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat | Aggro")
		float m_SecondaryObjectiveAggroWeight = 1.0f;


	void AddCharacterAbilities() override;

	//Methods
public:
	// Sets default values for this character's properties
	ABaseEnemy();

	//For when the enemy dies
	virtual void Server_OnDeath_Implementation(AActor* sourceActor) override;

	//Sets the current target
	virtual void SetCurrentTarget(AActor* target) { m_CurrentTarget = target; }
	UFUNCTION(BlueprintCallable)
		virtual AActor* GetCurrentTarget() { return m_CurrentTarget; }

	//Disables enemy in the world
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
		virtual void NetMulticast_DisableEnemy();

	//Enables enemy in the world
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
		virtual void NetMulticast_EnableEnemy(FVector newLocation);

	UFUNCTION(BlueprintCallable)
		virtual void DetermineAggro(float damageTaken, ABaseEntity* damageOwner);

	//---New Init
	UFUNCTION(BlueprintCallable)
		virtual void SetEnemyReferences(AWaveMaster* waveMaster, TArray<APlayerMech*> playerArray, AMainBase* base, TArray<ADrillObjective*> activeDrillList);

	UFUNCTION(BlueprintCallable)
		virtual void ResetEnemyReferences();

	UFUNCTION(BlueprintCallable)
		virtual bool ListOfTargetsAquired();

	UFUNCTION(BlueprintCallable)
		virtual void Attack();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
		virtual void NetMulticast_EnemyAudio(USoundBase* sound);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void SetCanAttackToTrue() { m_CanAttack = true; }
	virtual bool GetCanAttack() { return m_CanAttack; }

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};

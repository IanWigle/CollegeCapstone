// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interactable.h"
#include "Turret.generated.h"

UCLASS()
class OUTPOST_PRODUCTION_API ATurret : public AInteractable
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = Stats)
		bool m_IsLockedOntoEnemy = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Stats)
		float m_RotationSpeed = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Stats)
		float m_AttackCooldown = 5.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Stats)
		bool m_OverrideCurrentTarget = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Stats)
		int m_TurretRepairCost = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
		float m_TurretRotationSpeed;
	UPROPERTY(EditAnywhere)
		bool m_IsActive = true;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Targeting")
		AActor* m_CurrentTarget = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Targeting")
		TArray<AActor*> m_TargetList;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
		class USphereComponent* m_SightSphere;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Abilities")
		class ABaseWeapon* m_ActiveTurretWeapon;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Abilities")
		TArray<ABaseWeapon*> m_Weapons;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Abilities")
		TArray<TSubclassOf<ABaseWeapon>> m_WeaponClasses;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
		TSubclassOf<class UGameplayEffect> m_RepairEffect;

public:
	// Sets default values for this pawn's properties
	ATurret();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	UFUNCTION(BlueprintCallable)
		void ActivateWeapon();

	//Returns true if upgrade was applied
	bool ApplyUpgrade(TSubclassOf<UGameplayEffect> effect);
		
	UFUNCTION(NetMulticast, Reliable)
		void NetMulticast_BuildTurret(FTransform newTransform);
	UFUNCTION(NetMulticast, Reliable)
		void NetMulticast_RecoverTurret();
		void RepairTurret();

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

		void SwitchWeapon(int32 index);

	virtual void Server_OnDeath_Implementation(AActor* sourceActor) override;

	ABaseWeapon* GetWeapon() { return m_ActiveTurretWeapon; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
		void Server_DetermineNewTarget();
		bool Server_DetermineNewTarget_Validate() { return true; }

	UFUNCTION(BlueprintCallable)
		TMap<FString, int> GenerateWeaponSelectionArray();

	virtual void AddCharacterAbilities() override;

	friend class ABaseEnemy;

	void UpdateTurretBodyRotation(float DeltaTime);
};

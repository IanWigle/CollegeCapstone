// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/BaseEnemy.h"
#include "Hunter.generated.h"

/**
 * 
 */
UCLASS()
class OUTPOST_PRODUCTION_API AHunter : public ABaseEnemy
{
	GENERATED_BODY()
		//Members
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Max range")
		float m_MaxAttackRange = 9000.0f;;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Player Too Close")
		float m_PlayerIsCloseTeir1 = 6000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Player Too Close")
		float m_PlayerIsCloseTeir2 = 3000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Player Too Close")
		float m_PlayerIsCloseTeir3 = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "Attack")
		TSubclassOf<class UBaseGameplayAbility> m_HunterDash;

	UPROPERTY(BlueprintReadWrite, Category = "Abilites | Enable Dash")
		bool m_CanDash = true;

	UPROPERTY(BlueprintReadOnly, Category = "Abilites | Dash Cooldown")
		FTimerHandle m_DashCooldown;

private:
	//UPROPERTY(EditAnywhere)
	//	class UBehaviorTree* m_HunterBT;

	//Methods
public:
	AHunter();

	void SetEnemyReferences(AWaveMaster* waveMaster, TArray<APlayerMech*> playerArray, AMainBase* base, TArray<ADrillObjective*> activeDrillList) override;

	void Attack() override;
	virtual void Server_OnDeath_Implementation(AActor* sourceActor) override;

	UFUNCTION(BlueprintCallable)
		void MoveTowardTarget();
	UFUNCTION(BlueprintCallable)
		void MoveAwayFromTarget(int distanceFromPlayer);
	UFUNCTION(BlueprintCallable)
		int DistanceFromTarget();
	UFUNCTION(BlueprintCallable)
		void DashAway();

	void SetCanDashToTrue() { m_CanDash = true; }
	bool GetCanDash() { return m_CanDash; }

protected:
	void AddCharacterAbilities() override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Enums.h"
#include "Abilities/GameplayAbility.h"
#include "BaseEntity.generated.h"


/**
 * 
 */

UCLASS()
class OUTPOST_PRODUCTION_API ABaseEntity : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Replicated)
		bool m_bIsDead = false;
	/* 0 Means it is not in Test Mode. Anything other than 0 is a test mode.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Base Entity | Test")
		int m_InTestMode = 0;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Entity | Abilities")
		TArray<FGameplayAbilitySpec> m_AbilitySpecs;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Base Entity | Abilities")
		TArray<TSubclassOf<class UGameplayEffect>> m_StartupEffects;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Base Entity | Abilities")
		TSubclassOf<class UGameplayEffect> m_DeathEffect;

	UPROPERTY()
		class UBaseEntityAttributeSet* m_EntityAttributeSet;

	UPROPERTY(BlueprintReadOnly)
		UAbilitySystemComponent* m_EntityAbilitySystemComponent;

public:
	ABaseEntity();

	virtual void ResolveDamage(float Damage) {};
	virtual void RecoverHealth(float Health) {};
	virtual void RecoverArmor(float Armor) {};

	//Empty default implementation, OVERRIDE in subclasses
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_OnDeath(AActor* sourceActor);
	bool Server_OnDeath_Validate(AActor* sourceActor) { return true; }

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	 void OnDeathEvent();

    UFUNCTION(Server, Reliable, WithValidation)
        virtual void Server_OnDamageTaken(float damageAmount, ABaseEntity* damageOwner);
    bool Server_OnDamageTaken_Validate(float damageAmount, ABaseEntity* damageOwner) { return true; }

	virtual void AddStartUpEffects();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable)
		virtual void SetupEntitySpeed(float speed);

protected:
	virtual void BeginPlay();

    virtual int32 GetAbilityLevel(AbilityInputID AbilityID) const;
    // Grant abilities on the Server. The Ability Specs will be replicated to the owning client.
	//Override in subclasses
	virtual void AddCharacterAbilities() {};
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BaseGameplayAbility.generated.h"

/**
 * Used for our game. Should we want custom functionality in our abilities or helper functions they will go here.
 */
UCLASS()
class OUTPOST_PRODUCTION_API UBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual const FGameplayTagContainer* GetCooldownTags() const override;


	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "BaseEntityAttributeSet.h"
#include "BaseWeaponAttributeSet.h"
#include "EnemyDamageCalculation.generated.h"


struct AttributeCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(m_Health);

	DECLARE_ATTRIBUTE_CAPTUREDEF(m_Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(m_ArmorEffectiveness);

	DECLARE_ATTRIBUTE_CAPTUREDEF(m_UniversalDamage);
	AttributeCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseEntityAttributeSet, m_Health, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseEntityAttributeSet, m_Armor, Target, true);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseEntityAttributeSet, m_ArmorEffectiveness, Target, true);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseEntityAttributeSet, m_UniversalDamage, Source, false);

	}
};

/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class OUTPOST_PRODUCTION_API UEnemyDamageCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_UCLASS_BODY()
public:


		 virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyDamageCalculation.h"

UEnemyDamageCalculation::UEnemyDamageCalculation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AttributeCapture capturedAtts;

	RelevantAttributesToCapture.Add(capturedAtts.m_HealthDef);
	RelevantAttributesToCapture.Add(capturedAtts.m_ArmorDef);
	RelevantAttributesToCapture.Add(capturedAtts.m_ArmorEffectivenessDef);
	RelevantAttributesToCapture.Add(capturedAtts.m_UniversalDamageDef);
}

void UEnemyDamageCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	AttributeCapture attributes;

	UAbilitySystemComponent* targetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* sourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* sourceActor = sourceAbilitySystemComponent ? sourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	AActor* targetActor = targetAbilitySystemComponent ? targetAbilitySystemComponent->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* sourceTags = spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* targetTags = spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters evalParams;
	evalParams.SourceTags = sourceTags;
	evalParams.TargetTags = targetTags;

	float health = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(attributes.m_HealthDef, evalParams, health); //This passes our captured data into the float we just created.

	float armor = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(attributes.m_ArmorDef, evalParams, armor);

	float armorEffectiveness = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(attributes.m_ArmorEffectivenessDef, evalParams, armorEffectiveness);

	float damage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(attributes.m_UniversalDamageDef, evalParams, damage);

	damage -= armorEffectiveness;
	//damage < 1 ? damage = 1 : damage = damage;
	damage = FMath::Clamp(damage, 1.0f, damage);

	float remainingDamage = damage - armor;
	remainingDamage = FMath::Clamp(remainingDamage, 0.0f, remainingDamage);

	armor -= damage;
	armor < 0 ? armor = 0 : armor = armor;

	health -= remainingDamage;
	health < 0 ? health = 0 : health = health;


	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(attributes.m_ArmorProperty, EGameplayModOp::Additive, -damage));
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(attributes.m_HealthProperty, EGameplayModOp::Additive, -remainingDamage));
}
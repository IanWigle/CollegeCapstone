// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BaseWeaponAttributeSet.generated.h"


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 *
 */
UCLASS()
class OUTPOST_PRODUCTION_API UBaseWeaponAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_Damage)
        FGameplayAttributeData m_Damage;
    ATTRIBUTE_ACCESSORS(UBaseWeaponAttributeSet, m_Damage);

    UPROPERTY(BlueprintReadOnly, Category = "FireRate", ReplicatedUsing = OnRep_FireRate)
        FGameplayAttributeData m_FireRate;
    ATTRIBUTE_ACCESSORS(UBaseWeaponAttributeSet, m_FireRate);

    UPROPERTY(BlueprintReadOnly, Category = "ReloadSpeed", ReplicatedUsing = OnRep_ReloadSpeed)
        FGameplayAttributeData m_ReloadSpeed;
    ATTRIBUTE_ACCESSORS(UBaseWeaponAttributeSet, m_ReloadSpeed);

    UPROPERTY(BlueprintReadOnly, Category = "MagSize", ReplicatedUsing = OnRep_MagSize)
        FGameplayAttributeData m_MagSize;
    ATTRIBUTE_ACCESSORS(UBaseWeaponAttributeSet, m_MagSize);

    UPROPERTY(BlueprintReadOnly, Category = "CurrentAmmo", ReplicatedUsing = OnRep_CurrentAmmo)
        FGameplayAttributeData m_CurrentAmmo;
    ATTRIBUTE_ACCESSORS(UBaseWeaponAttributeSet, m_CurrentAmmo);

    UPROPERTY(BlueprintReadOnly, Category = "Range", ReplicatedUsing = OnRep_SoftRange)
        FGameplayAttributeData m_SoftRange;
    ATTRIBUTE_ACCESSORS(UBaseWeaponAttributeSet, m_SoftRange);

    UPROPERTY(BlueprintReadOnly, Category = "Range", ReplicatedUsing = OnRep_HardRange)
        FGameplayAttributeData m_HardRange;
	ATTRIBUTE_ACCESSORS(UBaseWeaponAttributeSet, m_HardRange);

	UPROPERTY(BlueprintReadOnly, Category = "Accuracy", ReplicatedUsing = OnRep_Spread)
		FGameplayAttributeData m_Spread;
	ATTRIBUTE_ACCESSORS(UBaseWeaponAttributeSet, m_Spread);

public:
    // AttributeSet Overrides
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    void AdjustAttributeForMaxChange(FGameplayAttributeData& affectedAttribute, const FGameplayAttributeData& maxAttribute, float newMaxValue, const FGameplayAttribute& affectedAttributeProperty);

    UFUNCTION()
        virtual void OnRep_Damage(const FGameplayAttributeData& Previous);

    UFUNCTION()
        virtual void OnRep_FireRate(const FGameplayAttributeData& Previous);

    UFUNCTION()
        virtual void OnRep_ReloadSpeed(const FGameplayAttributeData& Previous);

    UFUNCTION()
        virtual void OnRep_MagSize(const FGameplayAttributeData& Previous);

    UFUNCTION()
        virtual void OnRep_CurrentAmmo(const FGameplayAttributeData& Previous);

    UFUNCTION()
        virtual void OnRep_SoftRange(const FGameplayAttributeData& Previous);

    UFUNCTION()
		virtual void OnRep_HardRange(const FGameplayAttributeData& Previous);

	UFUNCTION()
		virtual void OnRep_Spread(const FGameplayAttributeData& Previous);


};

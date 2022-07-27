// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BaseEntityAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


/**
 *
 */
UCLASS()
class OUTPOST_PRODUCTION_API UBaseEntityAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health, meta = (AllowPrivateAccess = true))
        FGameplayAttributeData m_Health;
    ATTRIBUTE_ACCESSORS(UBaseEntityAttributeSet, m_Health);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
        FGameplayAttributeData m_MaxHealth;
    ATTRIBUTE_ACCESSORS(UBaseEntityAttributeSet, m_MaxHealth);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed", ReplicatedUsing = OnRep_Speed)
        FGameplayAttributeData m_Speed;
    ATTRIBUTE_ACCESSORS(UBaseEntityAttributeSet, m_Speed);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Armor", ReplicatedUsing = OnRep_Armor)
        FGameplayAttributeData m_Armor;
    ATTRIBUTE_ACCESSORS(UBaseEntityAttributeSet, m_Armor);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Armor", ReplicatedUsing = OnRep_MaxArmor)
        FGameplayAttributeData m_MaxArmor;
    ATTRIBUTE_ACCESSORS(UBaseEntityAttributeSet, m_MaxArmor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Armor", ReplicatedUsing = OnRep_ArmorEffectiveness)
		FGameplayAttributeData m_ArmorEffectiveness;
	ATTRIBUTE_ACCESSORS(UBaseEntityAttributeSet, m_ArmorEffectiveness);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aggro", ReplicatedUsing = OnRep_Aggro)
        FGameplayAttributeData m_Aggro;
    ATTRIBUTE_ACCESSORS(UBaseEntityAttributeSet, m_Aggro);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_UniversalDamage)
		FGameplayAttributeData m_UniversalDamage;
	ATTRIBUTE_ACCESSORS(UBaseEntityAttributeSet, m_UniversalDamage);

public:
	UBaseEntityAttributeSet();

	// AttributeSet Overrides
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    void AdjustAttributeForMaxChange(FGameplayAttributeData& affectedAttribute, const FGameplayAttributeData& maxAttribute, float newMaxValue, const FGameplayAttribute& affectedAttributeProperty);

    UFUNCTION()
        virtual void OnRep_Health(const FGameplayAttributeData& Previous);

    UFUNCTION()
        virtual void OnRep_MaxHealth(const FGameplayAttributeData& Previous);

    UFUNCTION()
        virtual void OnRep_Speed(const FGameplayAttributeData& Previous);

    UFUNCTION()
        virtual void OnRep_Armor(const FGameplayAttributeData& Previous);

    UFUNCTION()
        virtual void OnRep_MaxArmor(const FGameplayAttributeData& Previous);

	UFUNCTION()
		virtual void OnRep_ArmorEffectiveness(const FGameplayAttributeData& Previous);

    UFUNCTION()
        virtual void OnRep_Aggro(const FGameplayAttributeData& Previous);

	UFUNCTION()
		virtual void OnRep_UniversalDamage(const FGameplayAttributeData& Previous);
};
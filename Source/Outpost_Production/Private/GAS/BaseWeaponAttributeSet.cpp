// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeaponAttributeSet.h"
#include "UnrealNetwork.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "BaseWeapon.h"

void UBaseWeaponAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
}

void UBaseWeaponAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

    if (Data.EvaluatedData.Attribute == Getm_CurrentAmmoAttribute())
    {
        if (Getm_CurrentAmmo() <= 0.0f)
        {
			// Get the Target actor (this should be our owner)
			AActor* TargetActor = nullptr;
			//ABaseEntity = nullptr;
			if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
			{
				TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
			}

			if (TargetActor)
			{
				ABaseWeapon* owningWeapon = Cast<ABaseWeapon>(TargetActor);
				if (owningWeapon)
				{
					owningWeapon->Reload();
				}
			}

        }
    }
}

void UBaseWeaponAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UBaseWeaponAttributeSet, m_Damage, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UBaseWeaponAttributeSet, m_FireRate, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UBaseWeaponAttributeSet, m_ReloadSpeed, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UBaseWeaponAttributeSet, m_MagSize, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UBaseWeaponAttributeSet, m_CurrentAmmo, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UBaseWeaponAttributeSet, m_SoftRange, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseWeaponAttributeSet, m_HardRange, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseWeaponAttributeSet, m_Spread, COND_None, REPNOTIFY_Always);
}

void UBaseWeaponAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& affectedAttribute, const FGameplayAttributeData& maxAttribute, float newMaxValue, const FGameplayAttribute& affectedAttributeProperty)
{
}

void UBaseWeaponAttributeSet::OnRep_Damage(const FGameplayAttributeData& Previous)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseWeaponAttributeSet, m_Damage, Previous);
}

void UBaseWeaponAttributeSet::OnRep_FireRate(const FGameplayAttributeData& Previous)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseWeaponAttributeSet, m_FireRate, Previous);
}

void UBaseWeaponAttributeSet::OnRep_ReloadSpeed(const FGameplayAttributeData& Previous)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseWeaponAttributeSet, m_ReloadSpeed, Previous);
}

void UBaseWeaponAttributeSet::OnRep_MagSize(const FGameplayAttributeData& Previous)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseWeaponAttributeSet, m_MagSize, Previous);
}

void UBaseWeaponAttributeSet::OnRep_CurrentAmmo(const FGameplayAttributeData& Previous)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseWeaponAttributeSet, m_CurrentAmmo, Previous);
}

void UBaseWeaponAttributeSet::OnRep_SoftRange(const FGameplayAttributeData& Previous)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseWeaponAttributeSet, m_SoftRange, Previous);
}

void UBaseWeaponAttributeSet::OnRep_HardRange(const FGameplayAttributeData& Previous)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseWeaponAttributeSet, m_HardRange, Previous);
}

void UBaseWeaponAttributeSet::OnRep_Spread(const FGameplayAttributeData& Previous)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseWeaponAttributeSet, m_Spread, Previous);
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEntityAttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "UnrealNetwork.h"
#include "BaseEntity.h"
#include "PlayerMech.h"
#include "Turret.h"
#include "MainBase.h"
#include "BaseWeapon.h"
#include "Projectile.h"

UBaseEntityAttributeSet::UBaseEntityAttributeSet()
	: m_Health(1.0f)
	, m_MaxHealth(1.0f)
	, m_Speed(100.0f)
	, m_Armor(1.0f)
	, m_MaxArmor(1.0f)
	, m_ArmorEffectiveness(1.0f)
	, m_Aggro(1.0f)
	, m_UniversalDamage(0.0f)
{
}

void UBaseEntityAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == Getm_MaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(m_Health, m_MaxHealth, NewValue, Getm_HealthAttribute());
	}

	//Ensure we always clamp Health to max health and zero
	else if (Attribute == Getm_HealthAttribute())
	{
		UAbilitySystemComponent* abilityComp = GetOwningAbilitySystemComponent();

		const float maxHealth = m_MaxHealth.GetCurrentValue();

		if (NewValue > maxHealth)
		{
			NewValue = maxHealth;
		}
		else if (NewValue < 0.0f)
		{
			NewValue = 0.0f;
		}
	}

	else if (Attribute == Getm_MaxArmorAttribute())
	{
		AdjustAttributeForMaxChange(m_Armor, m_MaxArmor, NewValue, Getm_ArmorAttribute());
	}
}

void UBaseEntityAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	FGameplayTagContainer SpecAssetTags;
	Data.EffectSpec.GetAllAssetTags(SpecAssetTags);

	// Compute the delta between old and new, if it is available
	float DeltaValue = 0;
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
	{
		// If this was additive, store the raw delta value to be passed along later
		DeltaValue = Data.EvaluatedData.Magnitude;
	}

	// Get the Target actor (this should be our owner)
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	//ABaseEntity = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
	}

	//Get the source actor
	AActor* SourceActor = nullptr;
	AController* SourceController = nullptr;
	//ABaseEntity* SourceEntity = nullptr;
	if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
	{
		SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
		SourceController = Source->AbilityActorInfo->PlayerController.Get();
		if (SourceController == nullptr && SourceActor != nullptr)
		{
			if (APawn * Pawn = Cast<APawn>(SourceActor))
			{
				SourceController = Pawn->GetController();
			}
		}

		if (Context.GetEffectCauser())
		{
			SourceActor = Context.GetEffectCauser();
		}
	}

	//If this effect changed our Health attribute
	if (Data.EvaluatedData.Attribute == Getm_HealthAttribute())
	{
		//First handle HitMarkers if we know who made the damage
		if (SourceActor)
		{
			ABaseWeapon* sourceWeapon = Cast<ABaseWeapon>(SourceActor);

			if (sourceWeapon)
			{
				//If the weapon was owned by a Player than we can grab that player and spawn a HitMarker 
				APlayerMech* playerMech = Cast<APlayerMech>(sourceWeapon->GetOwner());
				if (playerMech)
				{
					if (Context.GetHitResult())
					{
						//Flip damage so it's less confusing
						playerMech->Client_SpawnHitMarker(DeltaValue * -1.0f, Context.GetHitResult()->ImpactPoint);
					}
					else if (TargetActor)
					{
						playerMech->Client_SpawnHitMarker(DeltaValue * -1.0f, TargetActor->GetActorLocation());
					}
				}
				//Otherwise if the weapon is owned by a turret, then we'll grab the owning player and spawn a HitMarker
				ATurret* playerTurret = Cast<ATurret>(sourceWeapon->GetOwner());
				if (playerTurret)
				{
					APlayerMech* owningMech = Cast<APlayerMech>(playerTurret->GetOwner());

					if (Context.GetHitResult())
					{
						owningMech->Client_SpawnHitMarker(DeltaValue * -1.0f, Context.GetHitResult()->ImpactPoint);
					}
					else if (TargetActor)
					{
						owningMech->Client_SpawnHitMarker(DeltaValue * -1.0f, TargetActor->GetActorLocation());
					}
				}
			}
		}

		ABaseEntity* targetEntity = Cast<ABaseEntity>(TargetActor);
		ABaseEntity* owningEntity = Cast<ABaseEntity>(SourceActor);
		if (targetEntity && owningEntity)
		{
			targetEntity->Server_OnDamageTaken(Data.EvaluatedData.Magnitude, owningEntity);
		}

		//If their health is now below 0, they're ded
		if (Getm_Health() <= 0.0f)
		{
			if (!SourceActor->HasAuthority())
				return;

			if (targetEntity->m_bIsDead == false)
			{
				targetEntity->Server_OnDeath(SourceActor);

				if (!targetEntity->ActorHasTag("MainBase") && !targetEntity->ActorHasTag("Turret"))
				{
					//Reset armor and health
					Setm_Health(Getm_MaxHealth());
					Setm_Armor(Getm_MaxArmor());
				}
			}
		}

		Setm_Health(FMath::Clamp(Getm_Health(), 0.0f, Getm_MaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == Getm_ArmorAttribute())
	{
		Setm_Armor(FMath::Clamp(Getm_Armor(), 0.0f, Getm_MaxArmor()));
	}
}

void UBaseEntityAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBaseEntityAttributeSet, m_Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseEntityAttributeSet, m_MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseEntityAttributeSet, m_Speed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseEntityAttributeSet, m_Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseEntityAttributeSet, m_MaxArmor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseEntityAttributeSet, m_ArmorEffectiveness, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseEntityAttributeSet, m_Aggro, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseEntityAttributeSet, m_UniversalDamage, COND_None, REPNOTIFY_Always);
}


void UBaseEntityAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& affectedAttribute, const FGameplayAttributeData& maxAttribute, float newMaxValue, const FGameplayAttribute& affectedAttributeProperty)
{
	UAbilitySystemComponent* abilityComp = GetOwningAbilitySystemComponent();
	const float currentMaxValue = maxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(currentMaxValue, newMaxValue) && abilityComp)
	{
		// Change current value to maintain the current Val / Max percent
		const float currentValue = affectedAttribute.GetCurrentValue();
		float newDelta = (currentMaxValue > 0.f) ? (currentValue * newMaxValue / currentMaxValue) - currentValue : newMaxValue;

		abilityComp->ApplyModToAttributeUnsafe(affectedAttributeProperty, EGameplayModOp::Additive, newDelta);
	}
}

void UBaseEntityAttributeSet::OnRep_Health(const FGameplayAttributeData& Previous)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseEntityAttributeSet, m_Health, Previous);
}

void UBaseEntityAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& Previous)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseEntityAttributeSet, m_MaxHealth, Previous);
}

void UBaseEntityAttributeSet::OnRep_Speed(const FGameplayAttributeData& Previous)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseEntityAttributeSet, m_Speed, Previous);
}

void UBaseEntityAttributeSet::OnRep_Armor(const FGameplayAttributeData& Previous)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseEntityAttributeSet, m_Armor, Previous);
}

void UBaseEntityAttributeSet::OnRep_MaxArmor(const FGameplayAttributeData& Previous)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseEntityAttributeSet, m_MaxArmor, Previous);
}

void UBaseEntityAttributeSet::OnRep_ArmorEffectiveness(const FGameplayAttributeData& Previous)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseEntityAttributeSet, m_ArmorEffectiveness, Previous);
}

void UBaseEntityAttributeSet::OnRep_Aggro(const FGameplayAttributeData& Previous)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseEntityAttributeSet, m_Aggro, Previous);
}

void UBaseEntityAttributeSet::OnRep_UniversalDamage(const FGameplayAttributeData& Previous)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseEntityAttributeSet, m_UniversalDamage, Previous);
}

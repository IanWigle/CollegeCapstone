// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameplayAbility.h"

const FGameplayTagContainer* UBaseGameplayAbility::GetCooldownTags() const
{
	return Super::GetCooldownTags();
}

void UBaseGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass());
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Cooldown")), 2.0f);
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}

	//Doing it ourselves
	//Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/SkeletalMeshActor.h"
#include "AbilitySystemInterface.h"
#include "BaseGameplayAbility.h"
#include "Sound/SoundCue.h"
#include "BaseWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ABaseWeapon : public ASkeletalMeshActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
		TSubclassOf<class UBaseGameplayAbility> m_WeaponAbility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		class UAudioComponent* m_AudioComponent;

protected:
	class UBaseWeaponAttributeSet* m_Attributes;
	FTimerHandle m_ReloadTimerHandle;
	FTimerHandle m_FireTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
		UAbilitySystemComponent* m_WeaponAbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
		TSubclassOf<class UBaseGameplayAbility> m_ReloadAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
		UAnimMontage* m_ReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
		USoundCue* m_FireSoundEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
		UParticleSystem* m_FireSystem;


	UPROPERTY(Replicated)
		FGameplayAbilitySpecHandle m_FireAbilityHandle;
	UPROPERTY(Replicated)
		FGameplayAbilitySpecHandle m_ReloadAbilityHandle;


private:
	void GiveAbilities();

public:
	ABaseWeapon();
	void BeginPlay();
	
	virtual void Reload();

	UFUNCTION(BlueprintCallable, Server, Reliable)
		virtual void Fire();

	UFUNCTION(BlueprintImplementableEvent)
		void WeaponFireEvent();

	UFUNCTION(NetMulticast, BlueprintCallable, Reliable)
		virtual void NetMulticast_WeaponFireStarted();

	UFUNCTION(BlueprintImplementableEvent)
		void WeaponHitEvent(FVector endLocation, AActor* hitActor);

	UFUNCTION(NetMulticast, BlueprintCallable, Reliable)
		virtual void NetMulticast_WeaponHit(FVector endLocation, AActor* hitActor);
	virtual void NetMulticast_WeaponHit_Implementation(FVector endLocation, AActor* hitActor) { WeaponHitEvent(endLocation, hitActor); }

	UFUNCTION(BlueprintImplementableEvent)
		void WeaponStopFireEvent();

	UFUNCTION(NetMulticast, BlueprintCallable, Reliable)
		virtual void NetMulticast_WeaponStopFire();
	virtual void NetMulticast_WeaponStopFire_Implementation() { WeaponStopFireEvent(); }

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};

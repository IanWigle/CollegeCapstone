// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Animation/AnimInstance.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_CustomMontageAndWait.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMontageWaitSimpleDelegate);
/**
 * 
 */
UCLASS()
class OUTPOST_PRODUCTION_API UAbilityTask_CustomMontageAndWait : public UAbilityTask
{
	GENERATED_UCLASS_BODY()
	
	UPROPERTY(BlueprintAssignable)
		FMontageWaitSimpleDelegate	OnCompleted;

	UPROPERTY(BlueprintAssignable)
		FMontageWaitSimpleDelegate	OnBlendOut;

	UPROPERTY(BlueprintAssignable)
		FMontageWaitSimpleDelegate	OnInterrupted;

	UPROPERTY(BlueprintAssignable)
		FMontageWaitSimpleDelegate	OnCancelled;

	UFUNCTION()
		void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
		void OnMontageInterrupted();

	UFUNCTION()
		void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "WeaponMontageAndWait",
		HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UAbilityTask_CustomMontageAndWait* CreateCustomMontageAndWaitProxy(UGameplayAbility* OwningAbility,
			FName TaskInstanceName, UAnimMontage* MontageToPlay, ABaseWeapon* WeaponToAnimate, float Rate = 1.f, FName StartSection = NAME_None, bool bStopWhenAbilityEnds = true, float AnimRootMotionTranslationScale = 1.f);

	virtual void Activate() override;

	/** Called when the ability is asked to cancel from an outside node. What this means depends on the individual task. By default, this does nothing other than ending the task. */
	virtual void ExternalCancel() override;

	virtual FString GetDebugString() const override;

private:

	virtual void OnDestroy(bool AbilityEnded) override;

	/** Checks if the ability is playing a montage and stops that montage, returns true if a montage was stopped, false if not. */
	bool StopPlayingMontage();

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	FDelegateHandle InterruptedHandle;

	UPROPERTY()
		UAnimMontage* MontageToPlay;

	UPROPERTY()
		float Rate;

	UPROPERTY()
		FName StartSection;

	UPROPERTY()
		float AnimRootMotionTranslationScale;

	UPROPERTY()
		bool bStopWhenAbilityEnds;

	UPROPERTY()
		class ABaseWeapon* m_WeaponToAnimate;
};

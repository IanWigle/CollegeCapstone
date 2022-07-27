// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Abilities/GameplayAbility.h"
#include "UpgradeWidget.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FUpgradeData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// The name of the upgrade
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName m_UpgradeName;

	// The description of the upgrade.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText m_UpgradeDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UGameplayEffect> m_AppliedEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FName> m_Prerequisites;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int m_UpgradeCost;
};


UCLASS()
class OUTPOST_PRODUCTION_API UUpgradeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FUpgradeData m_UpgradeData;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		bool m_HasBeenUsed = false;


	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere)
		class UButton* MainButton;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		class UTexture2D* ButtonEnabledImage;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		UTexture2D* ButtonDisabledImage;

public:
	UFUNCTION(BlueprintCallable)
		void ActivateUpgrade(class APlayerMech* player);

	UFUNCTION(BlueprintImplementableEvent)
		void UpdateWidgetColor();
};

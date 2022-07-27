// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HitMarkerWidget.generated.h"

/**
 * 
 */
UCLASS()
class OUTPOST_PRODUCTION_API UHitMarkerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* m_DamageText;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hit Info")
		float m_Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hit Info")
		FVector m_InitialWorld;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lifespan")
		float m_Lifespan = 2.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lifespan")
		FVector m_RandomDir = FVector::ZeroVector;

public:
	void Init(float damage, FVector worldPos);
};

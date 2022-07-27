// Fill out your copyright notice in the Description page of Project Settings.


#include "HitMarkerWidget.h"
#include "Components/TextBlock.h"

void UHitMarkerWidget::Init(float damage, FVector worldPos)
{
	//We want 1 degree of precision in the damage number
	m_Damage = FMath::RoundHalfFromZero(damage * 10.0f);
	m_Damage *= 0.1f;
	m_InitialWorld = worldPos;
	m_Lifespan = 2.0f;
	m_DamageText->SetText(FText::FromString(FString::SanitizeFloat(m_Damage)));

	m_RandomDir = FMath::VRand();
}

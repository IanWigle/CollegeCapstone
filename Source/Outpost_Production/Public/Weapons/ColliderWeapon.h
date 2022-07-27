// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "ColliderWeapon.generated.h"

/**
 * 
 */
UCLASS()
class AColliderWeapon : public ABaseWeapon
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Collision")
		UPrimitiveComponent* m_Collider;

public:
	AColliderWeapon();



};

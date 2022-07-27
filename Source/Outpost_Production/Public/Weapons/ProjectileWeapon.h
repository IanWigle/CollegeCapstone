// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseWeapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class OUTPOST_PRODUCTION_API AProjectileWeapon : public ABaseWeapon
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
		class AProjectile* GetAvailableProjectile();

	UFUNCTION(BlueprintCallable)
		void ReturnProjectile(AProjectile* projectile);
protected:
	void BeginPlay() override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Pool")
		TSubclassOf<AProjectile> m_ProjectileClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Object Pool")
		int m_NumProjectiles;
	
	static const int STATIC_MAX_PROJECTILES = 24;
private:
	AProjectile* m_ProjectilePool[STATIC_MAX_PROJECTILES];
	int m_AvailableProjectile = 0;
};

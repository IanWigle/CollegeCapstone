// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Projectile.h"
#include "Engine/World.h"

AProjectile* AProjectileWeapon::GetAvailableProjectile()
{
	if (m_AvailableProjectile >= 0 && m_ProjectilePool[m_AvailableProjectile])
	{
		AProjectile* returnVal = m_ProjectilePool[m_AvailableProjectile];
		m_ProjectilePool[m_AvailableProjectile] = nullptr;
		m_AvailableProjectile--;
		return returnVal;
	}

	return nullptr;
}

void AProjectileWeapon::ReturnProjectile(AProjectile* projectile)
{
	if (projectile != nullptr && m_AvailableProjectile < (STATIC_MAX_PROJECTILES - 1))
	{
		m_AvailableProjectile++;
		m_ProjectilePool[m_AvailableProjectile] = projectile;
		projectile->NetMulticast_Disable();
	}
}

void AProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (m_ProjectileClass.Get() == nullptr)
		return;

	if (!HasAuthority())
		return;

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	spawnParams.Owner = this;

	for (int i = 0; i < STATIC_MAX_PROJECTILES; i++)
	{
		m_ProjectilePool[i] = Cast<AProjectile>(GetWorld()->SpawnActor<AProjectile>(m_ProjectileClass, GetActorLocation(), FRotator::ZeroRotator, spawnParams));
		m_ProjectilePool[i]->NetMulticast_Disable();
	}

	m_AvailableProjectile = STATIC_MAX_PROJECTILES - 1;
}

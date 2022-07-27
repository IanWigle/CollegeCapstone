// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class AProjectile : public AActor
{
	GENERATED_BODY()
	
public:

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Config")
		float m_DropoffAmount;
	FTimerHandle m_DropoffTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Config")
		float m_MaxLifespan;
	FTimerHandle m_Lifespan;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Config")
		float m_ExplosionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Config")
		bool m_IsHeatSeaking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Weapon | Projectile")
		class UProjectileMovementComponent* m_ProjectileComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Weapon | Projectile")
		AActor* m_Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Weapon | Projectile")
		class USphereComponent* m_Collider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Weapon | Projectile")
		class USphereComponent* m_ExplosionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Weapon | Mesh")
		class UStaticMeshComponent* m_StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Config")
		float m_Damage;

private:
	TSubclassOf<class UGameplayEffect> m_DamageEffect;

public:	
	// Sets default values for this actor's properties
	AProjectile();

	UFUNCTION(BlueprintCallable)
		void AssignNewTarget(AActor* newTarget = nullptr);

	UFUNCTION(BlueprintCallable)
		AActor* GetTarget() {return m_Target;}

	UFUNCTION()
		void OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintImplementableEvent)
		void ApplyVisualEffectsOnSelf();

	UFUNCTION(BlueprintImplementableEvent)
		void ApplyVisualEffectsOnTarget(AActor* target);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
		virtual void NetMulticast_Disable();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
		virtual void NetMulticast_Enable(FVector newLocation);


	UFUNCTION(BlueprintCallable)
		void BeginLifeSpan();

protected:

	void LifeSpanExpired();

	UFUNCTION(NetMulticast, Reliable)
		void NetMulticast_ApplyVisualEffectsOnSelf();

	UFUNCTION(NetMulticast, Reliable)
		void NetMulticast_ApplyVisualEffectsOnTarget(AActor* target);
};

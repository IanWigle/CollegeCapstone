// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "UnrealNetwork.h"
#include "Engine/Engine.h"
#include "BaseEnemy.h"
#include "AbilitySystemComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "BaseWeapon.h"
#include "ProjectileWeapon.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_Collider = CreateDefaultSubobject<USphereComponent>("Collider");
	m_Collider->SetIsReplicated(true);
	m_Collider->SetGenerateOverlapEvents(true);
	m_Collider->SetCollisionProfileName("ProjectileCollision");
	m_Collider->OnComponentHit.AddDynamic(this, &AProjectile::OnCompHit);
	m_Collider->SetSimulatePhysics(false);
	m_Collider->SetEnableGravity(false);
	SetRootComponent(m_Collider);

	m_ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Movement");
	m_ProjectileComponent->SetUpdatedComponent(m_Collider);
	m_ProjectileComponent->SetIsReplicated(true);
	m_ProjectileComponent->bIsHomingProjectile = m_IsHeatSeaking;

	m_StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	m_StaticMesh->SetupAttachment(m_Collider);
	m_StaticMesh->SetIsReplicated(true);
	m_StaticMesh->SetSimulatePhysics(false);
	m_StaticMesh->SetEnableGravity(false);
	m_StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	m_ExplosionSphere = CreateDefaultSubobject<USphereComponent>("Explosion");
	m_ExplosionSphere->SetupAttachment(m_Collider);
	m_ExplosionSphere->SetIsReplicated(true);
	m_ExplosionSphere->SetGenerateOverlapEvents(true);
	m_ExplosionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	m_ExplosionSphere->SetSphereRadius(m_ExplosionRadius);
	m_ExplosionSphere->SetSimulatePhysics(false);
	m_ExplosionSphere->SetEnableGravity(false);

	SetActorEnableCollision(false);

	Tags.Add("Projectile");

	SetReplicates(true);
	SetReplicateMovement(true);

	static ConstructorHelpers::FClassFinder<UGameplayEffect> DamageEffectClass(TEXT("/Game/Blueprints/Weapons/GE_Damage"));
	if (DamageEffectClass.Class != NULL)
	{
		m_DamageEffect = DamageEffectClass.Class;
	}
}


void AProjectile::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//Exit if this isn't the server.
	if (!HasAuthority())
		return;

	TArray<AActor*> overlappingActors;
	m_ExplosionSphere->GetOverlappingActors(overlappingActors, ABaseEnemy::StaticClass());
	AProjectileWeapon* weapon = Cast<AProjectileWeapon>(GetOwner());

	if (!weapon) return;

	for (AActor* actor : overlappingActors)
	{
		FGameplayEffectContextHandle EffectContext = weapon->GetAbilitySystemComponent()->MakeEffectContext();
		FGameplayEffectSpecHandle effectHandle = weapon->GetAbilitySystemComponent()->MakeOutgoingSpec(m_DamageEffect, 1, EffectContext);
		effectHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Damage")), m_Damage);
		weapon->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*effectHandle.Data.Get());
		weapon->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*effectHandle.Data.Get(), Cast<ABaseEnemy>(actor)->GetAbilitySystemComponent());

		NetMulticast_ApplyVisualEffectsOnTarget(actor);
	}

	NetMulticast_ApplyVisualEffectsOnSelf();
	NetMulticast_Disable();

	weapon->ReturnProjectile(this);
}

void AProjectile::BeginLifeSpan()
{
	GetWorld()->GetTimerManager().SetTimer<AProjectile>(m_Lifespan, this, &AProjectile::LifeSpanExpired, m_MaxLifespan);
}

void AProjectile::LifeSpanExpired()
{
	//Any effects that happen if the missile doesn't hit anything should be placed here

	NetMulticast_Disable();
}

void AProjectile::NetMulticast_ApplyVisualEffectsOnSelf_Implementation()
{
	ApplyVisualEffectsOnSelf();
}

void AProjectile::NetMulticast_ApplyVisualEffectsOnTarget_Implementation(AActor* target)
{
	ApplyVisualEffectsOnTarget(target);
}

void AProjectile::NetMulticast_Disable_Implementation()
{
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
	m_ProjectileComponent->Deactivate();
	m_ProjectileComponent->HomingTargetComponent = nullptr;
}

void AProjectile::NetMulticast_Enable_Implementation(FVector newLocation)
{
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	SetActorEnableCollision(true);
	SetActorLocation(newLocation,false, nullptr, ETeleportType::TeleportPhysics);
}

void AProjectile::AssignNewTarget(AActor* newTarget /* = nullptr */)
{
	m_Target = newTarget;
	m_ProjectileComponent->Activate();
	m_ProjectileComponent->HomingTargetComponent = m_Target->GetRootComponent();
}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProjectile, m_ProjectileComponent);
	DOREPLIFETIME(AProjectile, m_Target);
	DOREPLIFETIME(AProjectile, m_StaticMesh);
	DOREPLIFETIME(AProjectile, m_Collider);
	DOREPLIFETIME(AProjectile, m_ExplosionSphere);
}
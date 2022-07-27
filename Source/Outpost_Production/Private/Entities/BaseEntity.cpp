// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEntity.h"
#include "Abilities/GameplayAbility.h"
#include "BaseEntityAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "UnrealNetwork.h"
#include "MechGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"

ABaseEntity::ABaseEntity()
{
	m_EntityAbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("Ability System");

	m_EntityAttributeSet = CreateDefaultSubobject<UBaseEntityAttributeSet>("Entity Attributes");

	Tags.Add("Entity");

	static ConstructorHelpers::FClassFinder<UGameplayEffect> EntityDeathTagClass(TEXT("/Game/Blueprints/Entities/Player/AbilitySystem/Effects/GE_EntityDeath"));
	if (EntityDeathTagClass.Class != NULL)
	{
		m_DeathEffect = EntityDeathTagClass.Class;
	}
}

UAbilitySystemComponent* ABaseEntity::GetAbilitySystemComponent() const
{
    return m_EntityAbilitySystemComponent;
}

void ABaseEntity::SetupEntitySpeed(float speed)
{
	UCharacterMovementComponent* moveComp = Cast<UCharacterMovementComponent>(GetCharacterMovement());
	moveComp->MaxWalkSpeed = speed;
	moveComp->MaxAcceleration = speed;
}

int32 ABaseEntity::GetAbilityLevel(AbilityInputID AbilityID) const
{
    return 1;
}


void ABaseEntity::BeginPlay()
{
	Super::BeginPlay();

	// Initialize the new Actor Info
	m_EntityAbilitySystemComponent->InitAbilityActorInfo(this, this);

	AddStartUpEffects();

	AddCharacterAbilities();
}

void ABaseEntity::AddStartUpEffects()
{
	if (!HasAuthority())
		return;

	// Now apply passives
	for (TSubclassOf<UGameplayEffect>& GameplayEffect : m_StartupEffects)
	{
		FGameplayEffectContextHandle EffectContext = m_EntityAbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		//Level is used to represent different balancing at different player counts.
		int level = 1;
		UMechGameInstance* gameInstance = Cast<UMechGameInstance>(GetGameInstance());
		if (gameInstance)
		{
			level = gameInstance->m_NumberOfConnections;
		}

		m_EntityAbilitySystemComponent->BP_ApplyGameplayEffectToSelf(GameplayEffect, level, EffectContext);
	}

	SetupEntitySpeed(m_EntityAbilitySystemComponent->GetSet<UBaseEntityAttributeSet>()->Getm_Speed());
}

void ABaseEntity::Server_OnDeath_Implementation(AActor* sourceActor)
{
	OnDeathEvent();

	if (m_bIsDead)
		return;

	m_bIsDead = true;

	if (m_DeathEffect.Get() == nullptr)
		return;

	FGameplayEffectContextHandle effectContext = m_EntityAbilitySystemComponent->MakeEffectContext();
	FGameplayEffectSpecHandle deathEffectHandle = m_EntityAbilitySystemComponent->MakeOutgoingSpec(m_DeathEffect, 1, effectContext);
	m_EntityAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*deathEffectHandle.Data.Get());
}

void ABaseEntity::Server_OnDamageTaken_Implementation(float damageAmount, ABaseEntity* damageOwner)
{
	UE_LOG(LogTemp, Log, TEXT("ABaseEntity::Server_OnDamageTaken_Implementation - %s took %f damage"), *GetName(), damageAmount);
}

void ABaseEntity::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseEntity, m_bIsDead);
	DOREPLIFETIME(ABaseEntity, m_InTestMode);
}
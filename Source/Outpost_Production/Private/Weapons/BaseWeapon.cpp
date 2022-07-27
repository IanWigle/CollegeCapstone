// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "BaseWeaponAttributeSet.h"
#include "Turret.h"
#include "Turret_AIController.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "BaseGameplayAbility.h"
#include "UnrealNetwork.h"

ABaseWeapon::ABaseWeapon()
{
	m_WeaponAbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("Ability System");
	m_WeaponAbilitySystemComponent->SetIsReplicated(true);
	m_WeaponAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	m_AudioComponent = CreateDefaultSubobject<UAudioComponent>("Audio Component");

	GetSkeletalMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetSkeletalMeshComponent()->SetEnableGravity(false);
	GetSkeletalMeshComponent()->SetIsReplicated(true);

	m_Attributes = CreateDefaultSubobject<UBaseWeaponAttributeSet>("Entity Attributes");

	GetSkeletalMeshComponent()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	SetReplicates(true);

	SetActorHiddenInGame(true);

	Tags.Add("Weapon");
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	m_WeaponAbilitySystemComponent->InitAbilityActorInfo(GetOwner(), this);

	GiveAbilities();
}

void ABaseWeapon::GiveAbilities()
{
	if (!HasAuthority())
		return;
	if (m_ReloadAbility)
		m_ReloadAbilityHandle = m_WeaponAbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(m_ReloadAbility, 1, INDEX_NONE, this));
	if (m_WeaponAbility)
		m_FireAbilityHandle = m_WeaponAbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(m_WeaponAbility, 1, INDEX_NONE, this));
}

void ABaseWeapon::Reload()
{
	//Activate Reload Ability
	m_WeaponAbilitySystemComponent->TryActivateAbility(m_ReloadAbilityHandle);
}

void ABaseWeapon::Fire_Implementation()
{
	//Activate Fire Ability
	if(!m_WeaponAbilitySystemComponent->TryActivateAbility(m_FireAbilityHandle))
	{
		UE_LOG(LogTemp, Warning, TEXT("ABaseWeapon.Failed to activate ability"));
	}
}

void ABaseWeapon::NetMulticast_WeaponFireStarted_Implementation()
{
	if (m_WeaponAbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Weapon.Reloading"))))
		return;

	WeaponFireEvent();
}

UAbilitySystemComponent* ABaseWeapon::GetAbilitySystemComponent() const
{
	return m_WeaponAbilitySystemComponent;
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseWeapon, m_ReloadAbilityHandle);
	DOREPLIFETIME(ABaseWeapon, m_FireAbilityHandle);

}
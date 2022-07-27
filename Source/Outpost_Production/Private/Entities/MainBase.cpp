// Fill out your copyright notice in the Description page of Project Settings.


#include "MainBase.h"
#include "PlayerMech.h"
#include "UnrealNetwork.h"
#include "Blueprint/UserWidget.h"
#include "Outpost_ProductionGameMode.h"
#include "Components/SkeletalMeshComponent.h"

#include "BaseEntityAttributeSet.h"
#include "BaseGameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilities/Public/GameplayAbilitySet.h"

#include "Outpost_ProductionGameState.h"

// Sets default values
AMainBase::AMainBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);

	Tags.Add("MainBase");

	m_CurrentDSIndex = 0;
	m_CurrentHealthSectionMax = 0;
}

void AMainBase::StartUpgrade(APlayerMech* player)
{
	if (m_UpgradeMenu && player)
	{
		MakeUpgradeMenu(player);
	}
}

TArray<FVector> AMainBase::GetTargetPoints()
{
	return m_PositionsForRushers;
}

// Called when the game starts or when spawned
void AMainBase::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->SetSkeletalMesh(m_BaseDamageStatesArray[m_CurrentDSIndex]);
	m_CurrentHealthSectionMax = GetAbilitySystemComponent()->GetSet<UBaseEntityAttributeSet>()->Getm_MaxHealth();

	if (!HasAuthority())
		return;

	AOutpost_ProductionGameMode* gameMode = Cast<AOutpost_ProductionGameMode>(GetWorld()->GetAuthGameMode());
	if (gameMode)
	{
		gameMode->SetMainObjective(this);
	}
}

void AMainBase::StopRegen()
{
	GetWorld()->GetTimerManager().ClearTimer(HealHandle);
	FString message = "AMainBase::StopRegen";
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, message);
}

void AMainBase::ResetHealWait()
{
	StopRegen();
	GetWorld()->GetTimerManager().SetTimer(HealWaitHandle, this, &AMainBase::StartRegen, m_CooldownWhenAttacked, false);
}

void AMainBase::StartRegen()
{
	GetWorld()->GetTimerManager().SetTimer(HealHandle, this, &AMainBase::RegenHealth, m_TimeBetweenRegenTick, true);
}

void AMainBase::RegenHealth()
{
	float health = GetAbilitySystemComponent()->GetSet<UBaseEntityAttributeSet>()->Getm_Health();
	if (health >= m_CurrentHealthSectionMax)
	{
		StopRegen();
	}

	else
	{
		if (RegenEffect.Get() != nullptr)
		{
			FGameplayEffectContextHandle EffectContext = m_EntityAbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle NewHandle = m_EntityAbilitySystemComponent->MakeOutgoingSpec(RegenEffect, 1, EffectContext);
			if (NewHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = m_EntityAbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), m_EntityAbilitySystemComponent);
			}
		}

		health = GetAbilitySystemComponent()->GetSet<UBaseEntityAttributeSet>()->Getm_Health();
		float maxHealth = GetAbilitySystemComponent()->GetSet<UBaseEntityAttributeSet>()->Getm_MaxHealth();

		FString message = "MainBase : Healed; now at " + FString::SanitizeFloat(health) + "\n MaxHealth: " + FString::SanitizeFloat(maxHealth);
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, message);
	}

}

// Called every frame
void AMainBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMainBase::Server_OnDeath_Implementation(AActor* sourceActor)
{
	//Super OnDeath called a the end of the function

	if (m_bIsDead)
		return;

	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("MainBase.OnDeath"));
		AOutpost_ProductionGameState* tempGameState = Cast<AOutpost_ProductionGameState>(GetWorld()->GetGameState());
		tempGameState->BaseDestroyed();
		tempGameState->m_DialogueMaster->Server_PlayDialogueForAll(DialogueEnum::BaseDestroyed);
	}

	Super::Server_OnDeath_Implementation(sourceActor);
}

void AMainBase::Server_OnDamageTaken_Implementation(float damageAmount, ABaseEntity* damageOwner)
{
	Super::Server_OnDamageTaken_Implementation(damageAmount, damageOwner);
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "AMainBase::Server_OnDamageTaken_Implementation");
	float health = GetAbilitySystemComponent()->GetSet<UBaseEntityAttributeSet>()->Getm_Health();
	float maxHealth = GetAbilitySystemComponent()->GetSet<UBaseEntityAttributeSet>()->Getm_MaxHealth();

	float percentHealth = health / maxHealth;
	float extraData = FMath::Clamp(percentHealth, 0.0f, 1.0f);
	float ChangeThreshold = 100 / (m_BaseDamageStatesArray.Num() - 1);

	ResetHealWait();

    //Only starts once below 75 hp
    //checks the percentage of the bases hp to the change threshold
	if (percentHealth <= (100.0f - ChangeThreshold * (m_CurrentDSIndex + 1)) / 100.0f)
	{
		m_CurrentHealthSectionMax = ((100.0f - ChangeThreshold * (m_CurrentDSIndex + 1)) / 100.0f) * maxHealth;

		NetMulticast_ChangeDamageState();
		if (HasAuthority())
		{
			UE_LOG(LogTemp, Warning, TEXT("AMainBase::OnDamageTaken_Implementation - server"));
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("AMainBase::OnDamageTaken_Implementation - client"));
	}
}

bool AMainBase::IsHealthRegenRunning()
{
	return GetWorld()->GetTimerManager().IsTimerActive(HealHandle);
}

bool AMainBase::IsWaitingToStartHeal()
{
	return GetWorld()->GetTimerManager().IsTimerActive(HealWaitHandle);
}

float AMainBase::GetRemainingHealthLockTime()
{
	if (IsHealthRegenRunning())
		return GetWorld()->GetTimerManager().GetTimerRemaining(HealWaitHandle);

	return 0.0f;
}

float AMainBase::GetRemainingTImeBetweenHeal()
{
	if (IsWaitingToStartHeal())
		return GetWorld()->GetTimerManager().GetTimerRemaining(HealWaitHandle);

	return 0.0f;
}

void AMainBase::NetMulticast_ChangeDamageState_Implementation()
{
	m_CurrentDSIndex++;
	GetMesh()->SetSkeletalMesh(m_BaseDamageStatesArray[m_CurrentDSIndex]);

	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("AMainBase::NetMulticast_ChangeDamageState_Implementation - client"));
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("AMainBase::NetMulticast_ChangeDamageState_Implementation - server"));
}



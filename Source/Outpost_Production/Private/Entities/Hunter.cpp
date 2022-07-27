// Fill out your copyright notice in the Description page of Project Settings.


#include "Hunter.h"
#include "Hunter.h"
#include "WaveMaster.h"
#include "AIController.h"
#include "Hunter_AIController.h"
#include "MainBase.h"
#include "PlayerMech.h"

#include "UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine/World.h"

#include "BaseEntityAttributeSet.h"
#include "BaseGameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilities/Public/GameplayAbilitySpec.h"

void AHunter::AddCharacterAbilities()
{
	Super::AddCharacterAbilities();

	if (!HasAuthority())
		return;

	if (m_EntityAbilitySystemComponent)
	{
		m_EntityAbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(m_HunterDash.GetDefaultObject(), 1, 1));
	}
}

AHunter::AHunter()
{
	Tags.Add("Hunter");
}

void AHunter::SetEnemyReferences(AWaveMaster* waveMaster, TArray<APlayerMech*> playerArray, AMainBase* base, TArray<ADrillObjective*> activeDrillList)
{
	Super::SetEnemyReferences(waveMaster, playerArray, base, activeDrillList);

	Cast<AHunter_AIController>(GetController())->RunBehavior();
}

void AHunter::MoveTowardTarget()
{
	if (m_CurrentTarget == nullptr)
		DetermineAggro(0, nullptr);

	FVector targetLoc = (m_CurrentTarget->GetActorTransform().GetLocation());
	targetLoc.Z = GetActorLocation().Z;

	AHunter_AIController* controller = Cast<AHunter_AIController>(GetController());
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(controller, targetLoc);
}

void AHunter::MoveAwayFromTarget(int distanceFromPlayer)
{
	AHunter_AIController* controller = Cast<AHunter_AIController>(GetController());


	if (distanceFromPlayer == 2)
	{
		FVector targetLoc = GetActorLocation();

		UAIBlueprintHelperLibrary::SimpleMoveToLocation(controller, targetLoc);
	}
	if (distanceFromPlayer == 3)
	{
		//move back
		FVector distance = m_CurrentTarget->GetActorLocation() - GetActorLocation();
		FVector targetLoc = GetActorLocation() - distance;
		targetLoc.Z = GetActorLocation().Z;

		UAIBlueprintHelperLibrary::SimpleMoveToLocation(controller, targetLoc);
	}
	else if (distanceFromPlayer == 4)
	{
		//jump really far back
		DashAway();

		UE_LOG(LogTemp, Warning, TEXT("Hunter : Jumped Back"));
	}
}

int AHunter::DistanceFromTarget()
{
	if (m_CurrentTarget == nullptr)
		return 0;

	float distance = FVector::Dist(GetActorLocation(), m_CurrentTarget->GetActorLocation());

	if (distance <= m_MaxAttackRange && distance > m_PlayerIsCloseTeir1)
	{
		//returns if within attacking range
		return 1;
	}
	else if (distance <= m_PlayerIsCloseTeir1 && distance > m_PlayerIsCloseTeir2)
	{
		//returns when the player is close, stop moving
		return 2;
	}
	else if (distance <= m_PlayerIsCloseTeir2 && distance > m_PlayerIsCloseTeir3)
	{
		//returns when the player is too close, move back
		return 3;
	}
	else if (distance <= m_PlayerIsCloseTeir3)
	{
		//returns when the player is extremely close, dash ability
		return 4;
	}
	else
	{
		return 0;
	}
}

void AHunter::Attack()
{
	Super::Attack();

	//Call this Hunters ability to attack
	if (m_EntityAbilitySystemComponent->FindAbilitySpecFromInputID(0))
		GetAbilitySystemComponent()->TryActivateAbility(m_EntityAbilitySystemComponent->FindAbilitySpecFromInputID(0)->Handle);

	m_CanAttack = false;
	GetWorld()->GetTimerManager().SetTimer(m_AttackCooldown, this, &AHunter::SetCanAttackToTrue, 5.0f, false);
	UE_LOG(LogTemp, Warning, TEXT("Hunter : Attack - attacked something"));
}

void AHunter::DashAway()
{
	if (m_CanDash == true)
	{
		m_CanDash = false;
	//Call this Hunters ability to Dash
	if (m_EntityAbilitySystemComponent->FindAbilitySpecFromInputID(1))
		GetAbilitySystemComponent()->TryActivateAbility(m_EntityAbilitySystemComponent->FindAbilitySpecFromInputID(1)->Handle);

	GetWorld()->GetTimerManager().SetTimer(m_DashCooldown, this, &AHunter::SetCanDashToTrue, 5.0f, false);
	}
}

void AHunter::Server_OnDeath_Implementation(AActor* sourceActor)
{
	//Super OnDeath called a the end of the function

	AHunter_AIController* controller = Cast<AHunter_AIController>(GetController());
	controller->HunterDeath();

	UE_LOG(LogTemp, Warning, TEXT("Hunter : Died"));

	m_WaveMaster->ReturnRanged(this);
	m_WaveMaster = nullptr;

	Super::Server_OnDeath_Implementation(sourceActor);
}
// Fill out your copyright notice in the Description page of Project Settings.

#include "Rusher.h"
#include "Outpost_ProductionGameMode.h"
#include "AIController.h"
#include "MainBase.h"
#include "Outpost_ProductionGameMode.h"
#include "AbilitySystemComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "UnrealNetwork.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine/World.h"
#include "WaveMaster.h"
#include "Outpost_ProductionGameState.h"


ARusher::ARusher()
{
	Tags.Add("Rusher");
}

void ARusher::SetEnemyReferences(AWaveMaster* waveMaster, TArray<APlayerMech*> playerArray, AMainBase* base, TArray<ADrillObjective*> activeDrillList)
{
	//calls the super of this function
	Super::SetEnemyReferences(waveMaster, playerArray, base, activeDrillList);

	//begins the behavior tree run
	AAIController* controller = Cast<AAIController>(GetController());

	if (controller)
		controller->RunBehaviorTree(m_RusherBehaviourTree);
}

void ARusher::Attack()
{	
	Super::Attack();

	//Call this rushers ability to attack
	if (m_EntityAbilitySystemComponent->FindAbilitySpecFromInputID(0))
		GetAbilitySystemComponent()->TryActivateAbility(m_EntityAbilitySystemComponent->FindAbilitySpecFromInputID(0)->Handle);

	m_CanAttack = false;
	GetWorld()->GetTimerManager().SetTimer(m_AttackCooldown, this, &ARusher::SetCanAttackToTrue, 5.0f, false);
}

void ARusher::Server_OnDeath_Implementation(AActor* sourceActor)
{
	//Super OnDeath called a the end of the function

	if (m_bIsDead)
		return;

	//Removes this rusher from the pack leaders pack when it dies
	if (m_InTestMode == 0)
	{
		AWaveMaster* waveMaster = Cast<AOutpost_ProductionGameState>(GetWorld()->GetGameState())->m_WaveMaster;
		waveMaster->ReturnRusher(this);
	}

	Super::Server_OnDeath_Implementation(sourceActor);
}

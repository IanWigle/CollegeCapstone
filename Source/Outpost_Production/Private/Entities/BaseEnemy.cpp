// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseEnemy.h"
#include "DrawDebugHelpers.h"

#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "Outpost_ProductionGameMode.h"
#include "MainBase.h"
#include "PlayerMech.h"
#include "Turret.h"
#include "..\..\Public\Objectives\DrillObjective.h"
#include "WaveMaster.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/Core/Public/Math/UnrealMathUtility.h"
#include "Templates/SubclassOf.h"
#include "UnrealNetwork.h"

#include "BaseEntityAttributeSet.h"
#include "BaseGameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilities/Public/GameplayAbilitySpec.h"

#include "Kismet/GameplayStatics.h"

#include "Outpost_ProductionGameState.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"


void ABaseEnemy::AddCharacterAbilities()
{
	if (!HasAuthority())
		return;
	if (m_EntityAbilitySystemComponent)
	{
		m_EntityAbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(EnemyAttack.GetDefaultObject(), 1, 0));
	}
}

// Sets default values
ABaseEnemy::ABaseEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);

	m_AudioComponent = CreateDefaultSubobject<UAudioComponent>("Audio System");
	m_AudioComponent->SetupAttachment(RootComponent);
	m_AudioComponent->bAutoActivate = false;
	m_AudioComponent->AdjustVolume(0.0f, 1.0f, EAudioFaderCurve::Linear);

	GetMesh()->SetEnableGravity(false);

	m_IsActive = false;

	Tags.Add("Enemy");

	//We are only calling the _implementation_ of this function, so it is not an actual RPC. This saves us having to do the actual RPC call.
	NetMulticast_DisableEnemy_Implementation();
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (m_EntityAbilitySystemComponent)
	{
		if (HasAuthority())
		{
			m_EntityAbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(EnemyAttack.GetDefaultObject(), 1, 0));
		}
	}
}

void ABaseEnemy::Server_OnDeath_Implementation(AActor* sourceActor)
{
	//Super OnDeath called a the end of the function
	if (m_bIsDead == true)
		return;


	int randDeathSound = FMath::RandRange(0, m_DeathSound.Num() - 1);
	NetMulticast_EnemyAudio(m_DeathSound[randDeathSound]);

	//clear their array of targets and players when they die
	m_ArrayOfPlayers.Empty();
	m_ActiveDrillList.Empty();
	m_TargetValues.Empty();

	AActor* killSource = (sourceActor->ActorHasTag("Weapon")) ? sourceActor->GetOwner() : sourceActor;

	APlayerMech* playerWithKill = nullptr;
	if (killSource->ActorHasTag("Player"))
	{
		playerWithKill = Cast<APlayerMech>(killSource);
	}
	else if (killSource->ActorHasTag("Turret"))
	{
		playerWithKill = Cast<APlayerMech>(killSource->GetOwner());
		Cast<ATurret>(killSource)->Server_DetermineNewTarget();
	}
	else
	{
		playerWithKill = Cast<APlayerMech>(killSource);
	}

	if (playerWithKill)
	{
		playerWithKill->AddResources(m_ResourcesWorth);
	}

	UE_LOG(LogTemp, Warning, TEXT("BaseEnemy : OnDeath()"));

	if (GetWorld()->GetTimerManager().IsTimerActive(m_AttackCooldown))
	{
		GetWorld()->GetTimerManager().ClearTimer(m_AttackCooldown);
	}

	AAIController* controller = Cast<AAIController>(GetController());
	if (controller)
	{
		controller->GetBlackboardComponent()->SetValueAsBool("IsActive", false);
	}

	NetMulticast_DisableEnemy_Implementation();

	Super::Server_OnDeath_Implementation(sourceActor);
}

void ABaseEnemy::NetMulticast_DisableEnemy_Implementation()
{
	//sets actor to not appear in game and disables their tick and collision
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
	GetCharacterMovement()->GravityScale = 0.0f;
	m_bIsDead = true;
	m_IsActive = false;
}

void ABaseEnemy::NetMulticast_EnableEnemy_Implementation(FVector newLocation)
{
	//sets actor to appear in the game and enables their tick and collision
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	SetActorEnableCollision(true);
	GetCharacterMovement()->GravityScale = 1.0f;
	SetActorLocation(newLocation, false, nullptr, ETeleportType::TeleportPhysics);
	m_bIsDead = false;

	//AWAKEND sound
	m_AudioComponent->Activate(true);
	NetMulticast_EnemyAudio(m_SpawnSound);
	//m_AudioComponent->SetSound(m_SpawnSound);
	//m_AudioComponent->Play();
	//m_AudioComponent->FadeOut(2.0f, 0.1f, EAudioFaderCurve::SCurve);
	m_IsActive = true;



	AAIController* controller = Cast<AAIController>(GetController());
	if (controller)
	{
		controller->GetBlackboardComponent()->SetValueAsBool("IsActive", true);
	}
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

// Called to bind functionality to input
void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseEnemy::Attack()
{
	//attack sound

	int randAttackSound = FMath::RandRange(0, m_AttackSound.Num() - 1);
	
	NetMulticast_EnemyAudio(m_AttackSound[randAttackSound]);
}

void ABaseEnemy::NetMulticast_EnemyAudio_Implementation(USoundBase* sound)
{
	m_AudioComponent->SetSound(sound);
	m_AudioComponent->Play(0.0f);
	m_AudioComponent->StopDelayed(2.0f);
	//m_AudioComponent->FadeOut(2.0f, 0.1f, EAudioFaderCurve::SCurve);
}

void ABaseEnemy::DetermineAggro(float damageTaken, ABaseEntity* damageOwner)
{
	//Clear current set of possible Targets
	m_TargetValues.Empty();

	for (int i = 0; i < m_ArrayOfPlayers.Num(); i++)
	{
		FTarget newTarget;
		newTarget.TargetEntity = m_ArrayOfPlayers[i];

		//If they are dead, don't attack them
		if (m_ArrayOfPlayers[i]->m_bIsDead == true)
		{
			newTarget.AggroValue = 0.0f;
		}
		//If they recently shot us, use their damage as aggro value
		else if (m_ArrayOfPlayers[i] == damageOwner)
		{
			newTarget.AggroValue = damageTaken * 100.0f;
		}
		//Otherwise use the base aggro value
		else
		{
			newTarget.AggroValue = m_ArrayOfPlayers[i]->GetAbilitySystemComponent()->GetSet<UBaseEntityAttributeSet>()->Getm_Aggro();
		}
		//Adjust for player mech aggro weight
		newTarget.AggroValue *= m_PlayerMechAggroWeight;

		m_TargetValues.Add(newTarget);

		//Now do Player's Turret
		newTarget.TargetEntity = m_ArrayOfPlayers[i]->GetTurret();
		if (newTarget.TargetEntity->m_bIsDead == true)
		{
			newTarget.AggroValue = 0.0f;
		}
		else if (newTarget.TargetEntity == damageOwner)
		{
			newTarget.AggroValue = damageTaken * 100.0f;
		}
		else
		{
			newTarget.AggroValue = newTarget.TargetEntity->GetAbilitySystemComponent()->GetSet<UBaseEntityAttributeSet>()->Getm_Aggro();
		}

		//Adjust for turret aggro weight
		newTarget.AggroValue *= m_TurretAggroWeight;

		m_TargetValues.Add(newTarget);
	}

	//Cycle through all active Drills
	for (int i = 0; i < m_ActiveDrillList.Num(); i++)
	{
		FTarget newTarget;
		newTarget.TargetEntity = m_ActiveDrillList[i];
		if (m_ActiveDrillList[i]->m_bIsDead == true)
		{
			newTarget.AggroValue = 0.0f;
		}
		else
		{
			newTarget.AggroValue = m_ActiveDrillList[i]->GetAbilitySystemComponent()->GetSet<UBaseEntityAttributeSet>()->Getm_Aggro();
		}
		//Adjust for secondary objective aggro weight
		newTarget.AggroValue *= m_SecondaryObjectiveAggroWeight;

		m_TargetValues.Add(newTarget);
	}

	//Finally, add the base
	FTarget baseTarget;
	baseTarget.TargetEntity = m_MainBase;
	if (m_MainBase->m_bIsDead == true)
	{
		baseTarget.AggroValue = 0.0f;
	}
	else
	{
		baseTarget.AggroValue = m_MainBase->GetAbilitySystemComponent()->GetSet<UBaseEntityAttributeSet>()->Getm_Aggro();
	}

	//Adjust for main base aggro weight
	baseTarget.AggroValue *= m_MainBaseAggroWeight;

	m_TargetValues.Add(baseTarget);

	float highestAggro = 0.0f;
	int highestAggroIndex = -1;

	for (int i = 0; i < m_TargetValues.Num(); i++)
	{
		float dist = FVector::Dist(this->GetActorLocation(), m_TargetValues[i].TargetEntity->GetActorLocation());

		m_TargetValues[i].AggroValue /= dist;

		if (m_TargetValues[i].AggroValue > highestAggro)
		{
			highestAggro = m_TargetValues[i].AggroValue;
			highestAggroIndex = i;
		}
	}

	//Assuming we didn't somehow end up with garbage data
	if (highestAggroIndex > -1)
	{
		SetCurrentTarget(m_TargetValues[highestAggroIndex].TargetEntity);
		//UE_LOG(LogTemp, Display, TEXT("BaseEnemy::DetermingAggro - new aggro target name: %s"), *m_TargetValues[highestAggroIndex].TargetEntity->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BaseEnemy::DetermineAggro - no highestAggroIndex"));
	}
}

//---New Init
void ABaseEnemy::SetEnemyReferences(AWaveMaster* waveMaster, TArray<APlayerMech*> playerArray, AMainBase* base, TArray<ADrillObjective*> activeDrillList)
{
	//sets the members of base enemy to the references passed in
	if (m_WaveMaster != waveMaster)
		m_WaveMaster = waveMaster;

	if (m_ArrayOfPlayers != playerArray)
		m_ArrayOfPlayers = playerArray;

	if (m_MainBase != base)
		m_MainBase = base;

	if (m_ActiveDrillList != activeDrillList)
		m_ActiveDrillList = activeDrillList;
}

void ABaseEnemy::ResetEnemyReferences()
{
	//calls the wavemasters reset enemy references
	m_WaveMaster->ResetEnemyReferences(this);
}

bool ABaseEnemy::ListOfTargetsAquired()
{
	////checks if the player list is empty
	//if (m_ArrayOfPlayers.Num() == 0)
	//{
	//	//reset references if empty and return false
	//	ResetEnemyReferences();
	//	return false;
	//}
	////else it will set the target list for the enemy
	//else
	//{
	//	//sets the list of targets
	//	int playerCounter = 0;

	//	FTarget newTarget;

	//	newTarget.TargetEntity = m_MainBase;
	//	UAbilitySystemComponent* component = m_MainBase->GetAbilitySystemComponent();

	//	if (component)
	//	{
	//		newTarget.m_BaseAggroValue = component->GetSet<UBaseEntityAttributeSet>()->Getm_Aggro();
	//		m_TargetValues.Add(newTarget);
	//	}

	//	if (m_ActiveDrillList.Num() > 0)
	//	{
	//		for (int i = 0; i < m_ActiveDrillList.Num(); i++)
	//		{
	//			newTarget.TargetEntity = m_ActiveDrillList[i];
	//			component = m_ActiveDrillList[i]->GetAbilitySystemComponent();
	//			newTarget.m_BaseAggroValue = component->GetSet<UBaseEntityAttributeSet>()->Getm_Aggro();
	//			m_TargetValues.Add(newTarget);
	//		}
	//	}

	//	for (int i = 0; i < m_ArrayOfPlayers.Num(); i++)
	//	{
	//		//gets the player
	//		newTarget.TargetEntity = m_ArrayOfPlayers[i];
	//		component = m_ArrayOfPlayers[i]->GetAbilitySystemComponent();
	//		newTarget.m_BaseAggroValue = component->GetSet<UBaseEntityAttributeSet>()->Getm_Aggro();
	//		m_TargetValues.Add(newTarget);
	//		playerCounter++;

	//		if (m_ArrayOfPlayers[i]->GetTurret() != nullptr && m_ArrayOfPlayers[i]->GetTurret()->IsActorTickEnabled() == true)
	//		{

	//			//Gets the players turret
	//			newTarget.TargetEntity = m_ArrayOfPlayers[i]->GetTurret();
	//			component = m_ArrayOfPlayers[i]->GetAbilitySystemComponent();
	//			newTarget.m_BaseAggroValue = component->GetSet<UBaseEntityAttributeSet>()->Getm_Aggro();
	//			m_TargetValues.Add(newTarget);
	//		}
	//	}

	//	//returns true if player count is equal to the amount of players in the game, else returns false
	//	if (playerCounter == m_ArrayOfPlayers.Num())
	//		return true;
	//	else
	//		return false;
	//}

	return true;
}

void ABaseEnemy::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseEnemy, m_CurrentTarget);
	DOREPLIFETIME(ABaseEnemy, m_ArrayOfPlayers);
	DOREPLIFETIME(ABaseEnemy, m_WaveMaster);
	DOREPLIFETIME(ABaseEnemy, m_TargetValues);
	DOREPLIFETIME(ABaseEnemy, m_MainBase);
	DOREPLIFETIME(ABaseEnemy, m_ActiveDrillList);
	DOREPLIFETIME(ABaseEnemy, m_SpawnSound);
	DOREPLIFETIME(ABaseEnemy, m_AttackSound);
	DOREPLIFETIME(ABaseEnemy, m_DeathSound);
}
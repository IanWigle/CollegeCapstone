// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "UnrealNetwork.h"
#include "BaseWeapon.h"
#include "BaseGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BaseGameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Turret_AIController.h"
#include "GameplayAbilities/Public/GameplayAbilitySpec.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerMech.h"
#include "BrainComponent.h"


// Sets default values
ATurret::ATurret()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_SightSphere = CreateDefaultSubobject<USphereComponent>("Sight Sphere");
	m_SightSphere->SetupAttachment(RootComponent);
	m_SightSphere->OnComponentBeginOverlap.AddDynamic(this, &ATurret::OnOverlapBegin);
	m_SightSphere->OnComponentEndOverlap.AddDynamic(this, &ATurret::EndOverLap);
	m_SightSphere->ComponentTags.Add("Sight");

	GetMesh()->SetEnableGravity(false);
	GetCharacterMovement()->GravityScale = 0.0f;

	SetReplicates(true);

	m_bIsDead = false;

	Tags.Add("Turret");
}

void ATurret::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (m_CurrentTarget == nullptr && OtherActor->ActorHasTag("Enemy"))
	{
		m_CurrentTarget = OtherActor;
		m_TargetList.Add(OtherActor);

		auto controller = Cast<AAIController>(GetController());

		if (controller)
		{
			if (controller->GetBlackboardComponent())
				controller->GetBlackboardComponent()->SetValueAsObject("CurrentTarget", m_CurrentTarget);
		}
	}
	else if (m_TargetList.Contains(OtherActor) == false && OtherActor->ActorHasTag("Enemy"))
	{
		m_TargetList.Add(OtherActor);
		if (m_OverrideCurrentTarget)
			m_CurrentTarget = OtherActor;
	}
 }

void ATurret::EndOverLap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (m_CurrentTarget == OtherActor)
		m_CurrentTarget = nullptr;

	m_TargetList.Remove(OtherActor);

	Server_DetermineNewTarget();
}

void ATurret::Server_OnDeath_Implementation(AActor* sourceActor)
{
	//Super OnDeath called a the end of the function

	UBlackboardComponent* blackboard = Cast<AAIController>(GetController())->GetBlackboardComponent();

	if (blackboard)
	{
		blackboard->SetValueAsBool(FName("IsActive"), false);
	}
	
	if (m_bIsDead)
		return;

	UE_LOG(LogTemp, Warning, TEXT("ATurret.OnDeath"));

	Super::Server_OnDeath_Implementation(sourceActor);
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
}

TMap<FString, int> ATurret::GenerateWeaponSelectionArray()
{
	TMap<FString, int> availableWeapons;
	for (int i = 0; i < m_Weapons.Num(); i++)
	{
		FGameplayTagContainer weaponTagContainer;
		m_Weapons[i]->GetAbilitySystemComponent()->GetOwnedGameplayTags(weaponTagContainer);

		FGameplayTagContainer turretTagContainer;
		GetAbilitySystemComponent()->GetOwnedGameplayTags(turretTagContainer);

		if (turretTagContainer.HasAll(weaponTagContainer))
			availableWeapons.Add(m_Weapons[i]->GetName(), i);
	}

	return availableWeapons;
}

void ATurret::AddCharacterAbilities()
{
	if (!HasAuthority())
		return;

	FActorSpawnParameters weaponSpawnParams;
	weaponSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	weaponSpawnParams.Owner = this;

	FAttachmentTransformRules transformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, false);

	if (m_WeaponClasses.Num() > 0)
	{
		for (int i = 0; i < m_WeaponClasses.Num(); i++)
		{
			m_Weapons.Add(Cast<ABaseWeapon>(GetWorld()->SpawnActor<ABaseWeapon>(m_WeaponClasses[i], GetActorLocation(), FRotator::ZeroRotator, weaponSpawnParams)));
			m_Weapons[i]->AttachToComponent(GetMesh(), transformRules, "Turret_WeaponSocket");
			m_Weapons[i]->SetActorHiddenInGame(true);
		}

		m_ActiveTurretWeapon = (m_Weapons.IsValidIndex(0)) ? m_Weapons[0] : nullptr;

		if (m_ActiveTurretWeapon != nullptr)
		{
			m_ActiveTurretWeapon->SetActorHiddenInGame(false);



			// Initialize the new Actor Info
			m_EntityAbilitySystemComponent->InitAbilityActorInfo(this, this);
		}
	}	
}

void ATurret::UpdateTurretBodyRotation(float DeltaTime)
{
	// Get the turret's AI controller.
		ATurret_AIController* controller = Cast<ATurret_AIController>(GetController());

	// If the controller is valid.
	if (controller)
	{
		// Get the blackboards target object from the CurrentTarget key.
		ABaseEntity* target = Cast<ABaseEntity>(controller->GetBlackboardComponent()->GetValueAsObject("CurrentTarget"));

		// If the target is valid.
		if (target)
		{
			// Get the target's location and this object's location
			FVector targetLocation = target->GetActorLocation();
			FVector originLocation = GetActorLocation();

			// Calculate the lookAtRotation with the originLocation and targetLocation
			FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(originLocation, targetLocation);

			// Set the Roll and Pitch of the rotation to 0.0f as we only want it to rotate on the yaw.
			lookAtRotation.Roll = 0.0f;
			lookAtRotation.Pitch = 0.0f;

			// Calculate the new rotation to apply with deltatime and our rotation speed.
			FRotator newRotation = UKismetMathLibrary::RInterpTo(
				GetActorRotation(), lookAtRotation, DeltaTime, m_TurretRotationSpeed);

			// Apply the new rotation.
			SetActorRotation(newRotation);
		}
	}
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority())
		return;

	UpdateTurretBodyRotation(DeltaTime);
}

void ATurret::ActivateWeapon()
{
	if (!HasAuthority())
		return;

	m_ActiveTurretWeapon->Fire();
 }


bool ATurret::ApplyUpgrade(TSubclassOf<UGameplayEffect> effect)
{
	bool didUpgrade = false;

	FGameplayEffectContextHandle turretEffectContext = m_EntityAbilitySystemComponent->MakeEffectContext();

	if (turretEffectContext.IsValid())
	{
		turretEffectContext.AddSourceObject(this);

		m_EntityAbilitySystemComponent->BP_ApplyGameplayEffectToSelf(effect, 1, turretEffectContext);
		didUpgrade = true;
	}

	for (ABaseWeapon* turretWeapon : m_Weapons)
	{
		if (!turretWeapon)
		{
			UE_LOG(LogTemp, Warning, TEXT("APlayerMech : Server_Upgrade : turretWeapon not valid"));
		}

		UAbilitySystemComponent* weaponComponent = turretWeapon->GetAbilitySystemComponent();

		if (!weaponComponent)
		{
			UE_LOG(LogTemp, Warning, TEXT("APlayerMech : Server_Upgrade : weaponcomponent not valid"));
		}

		FGameplayEffectContextHandle weaponeffectContext = weaponComponent->MakeEffectContext();

		if (weaponeffectContext.IsValid())
		{
			weaponeffectContext.AddSourceObject(this);

			weaponComponent->BP_ApplyGameplayEffectToSelf(effect, 1, weaponeffectContext);
			didUpgrade = true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("APlayerMech : Server_Upgrade : weaponEffectContext not valid"));
		}
	}

	return didUpgrade;
}

void ATurret::NetMulticast_BuildTurret_Implementation(FTransform newTransform)
{
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	SetActorLocation(newTransform.GetLocation());
	SetActorEnableCollision(true);
	if (m_ActiveTurretWeapon)
	{
		m_ActiveTurretWeapon->SetActorHiddenInGame(false);
		m_ActiveTurretWeapon->SetActorTickEnabled(true);
	}
	m_IsActive = true;

	ATurret_AIController* controller = Cast<ATurret_AIController>(GetController());
	if (controller)
	{
		controller->GetBlackboardComponent()->SetValueAsBool("IsActive", m_IsActive);
	}

	UE_LOG(LogTemp, Warning, TEXT("ATurret.NetMulticast_BuildTurret_Implementation"));
}

void ATurret::RepairTurret()
{
	FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponent()->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	GetAbilitySystemComponent()->BP_ApplyGameplayEffectToSelf(m_RepairEffect, 1, EffectContext);

	ATurret_AIController* controller = Cast<ATurret_AIController>(GetController());
	if (controller)
	{
		controller->GetBlackboardComponent()->SetValueAsBool("IsActive", m_IsActive);
	}
}

void ATurret::NetMulticast_RecoverTurret_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ATurret.NetMulticast_RecoverTurret_Implementation"));
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
	if (m_ActiveTurretWeapon)
	{
		m_ActiveTurretWeapon->SetActorHiddenInGame(true);
		m_ActiveTurretWeapon->SetActorTickEnabled(false);
	}
	m_IsActive = false;

	ATurret_AIController* controller = Cast<ATurret_AIController>(GetController());

	if (controller)
		controller->GetBlackboardComponent()->SetValueAsBool("IsActive", m_IsActive);


	if (m_ActiveTurretWeapon)
	{
		m_ActiveTurretWeapon->SetActorHiddenInGame(true);
		m_ActiveTurretWeapon->SetActorTickEnabled(false);
	}
}

void ATurret::Server_DetermineNewTarget_Implementation()
{
	if (m_TargetList.Num() > 0)
	{
		float lowestDistance = TNumericLimits<float>::Max();
		int lowestDistanceIndex = 0;

		for (int i = 0; i < m_TargetList.Num(); i++)
		{
			float newLowDistance = FVector::Dist(GetActorLocation(), m_TargetList[i]->GetActorLocation());
			if (newLowDistance < lowestDistance)
			{
				lowestDistance = newLowDistance;
				lowestDistanceIndex = i;
			}
		}

		if (lowestDistance != TNumericLimits<float>::Max())
		{
			m_CurrentTarget = m_TargetList[lowestDistanceIndex];
		}

		auto controller = Cast<AAIController>(GetController());

		if (controller)
		{
			controller->GetBlackboardComponent()->SetValueAsObject("CurrentTarget", m_CurrentTarget);
		}
	}
	else
	{
		m_CurrentTarget = nullptr;

		auto controller = Cast<AAIController>(GetController());

		if (controller)
		{
			controller->GetBlackboardComponent()->SetValueAsObject("CurrentTarget", nullptr);
		}
	}
}

void ATurret::SwitchWeapon(int32 index)
{
	UE_LOG(LogTemp, Warning, TEXT("ATurret::Server_SwitchWeapon_Implementation - function called"));

	m_ActiveTurretWeapon->SetActorHiddenInGame(true);

	//Check if the turret is currently placed before making the new weapon mesh visible)
	//Tick will be disabled if the turret is currently hidden
	if (IsActorTickEnabled())
	{
		m_Weapons[index]->SetActorHiddenInGame(false);
	}
	m_ActiveTurretWeapon = m_Weapons[index];

	auto controller = Cast<AAIController>(GetController());

	if (controller)
	{
		controller->GetBlackboardComponent()->SetValueAsClass("CurrentWeaponAbility", m_ActiveTurretWeapon->m_WeaponAbility);
	}
}

void ATurret::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATurret, m_CurrentTarget);
	DOREPLIFETIME(ATurret, m_ActiveTurretWeapon);
	DOREPLIFETIME(ATurret, m_Weapons);
}
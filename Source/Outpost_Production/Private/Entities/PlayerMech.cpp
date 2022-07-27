// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMech.h"
#include "MainBase.h"
#include "Interactable.h"
#include "Turret.h"
#include "BaseWeapon.h"
#include "BaseEnemy.h"
#include "Rusher.h"
#include "Hunter.h"
#include "Turret_AIController.h"

#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerState.h"
#include "TimerManager.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/AudioComponent.h"
#include "Components/TextBlock.h"

#include "BaseGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "HitMarkerWidget.h"
#include "SubtitleWidget.h"
#include "BaseEntityAttributeSet.h"

#include "UnrealNetwork.h"
#include "Math/NumericLimits.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/Engine.h"

#include "DrawDebugHelpers.h"

#include "Outpost_ProductionGameMode.h"
#include "Outpost_ProductionGameState.h"
#include "MechGameInstance.h"
#include "PlayerMechController.h"

#include "WaveMaster.h"

// Sets default values
APlayerMech::APlayerMech()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create and setup the Camera Arm.
	// Create a camera boom (pulls in towards the player if there is a collision)
	m_CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	m_CameraArm->SetupAttachment(RootComponent);
	m_CameraArm->TargetArmLength = 300.0f; // The camera follows at this distance behind the character
	m_CameraArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create and setup the follow camera.
	m_FollowCamera = CreateDefaultSubobject<UCameraComponent>("Follow Camera");
	m_FollowCamera->SetupAttachment(m_CameraArm, USpringArmComponent::SocketName);
	m_FollowCamera->bUsePawnControlRotation = false;

	GetCapsuleComponent()->SetGenerateOverlapEvents(true);

	m_InteractBounds = CreateDefaultSubobject<UCapsuleComponent>("Interaction Bounds");
	m_InteractBounds->SetupAttachment(RootComponent);
	m_InteractBounds->OnComponentBeginOverlap.AddDynamic(this, &APlayerMech::OnInteractionOverlapBegin);
	m_InteractBounds->OnComponentEndOverlap.AddDynamic(this, &APlayerMech::EndInteractionOverLap);

	m_DialogueComponent = CreateDefaultSubobject<UAudioComponent>("Dialogue");
	m_DialogueComponent->SetupAttachment(RootComponent);
	m_DialogueComponent->SetIsReplicated(true);
	m_DialogueComponent->OnAudioFinished.AddDynamic(this, &APlayerMech::Client_NextDialogueInQue);

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	m_BaseLookUpRate = 20.0f;
	m_BaseTurnRate = 20.0f;

	//0
	m_TutorialSteps.Add("Use the [Arrow Keys] to move your mech, and the [Mouse] to look around. You can also use boosters to quickly get in or out of position with the [Space] button.");
	m_StepsComplete.Add(false);

	//1
	m_TutorialSteps.Add("Use [LMB] and [RMB] to fire your left and right weapons.");
	m_StepsComplete.Add(false);

	//2
	m_TutorialSteps.Add("Your weapons reload automatically when emptied, or you can press [R] to reload both weapons manually.");
	m_StepsComplete.Add(false);

	//3
	m_TutorialSteps.Add("Press [Q] to activate your special weapons. These special weapons generate a lot of heat, and are put on a short cooldown after use.");
	m_StepsComplete.Add(false);

	//4
	m_TutorialSteps.Add("You can interact with the research outpost by pressing [E] near it. The research outpost can display and change your current weapon loadout, as well as provide you with new weapons and upgrades in exchange for resources.");
	m_StepsComplete.Add(false);

	//5
	m_TutorialSteps.Add("We�ve equipped your mech with a deployable turret to aid you. Press [F] to deploy your turret in front of you on a flat surface. Once deployed, you can interact and repair your turret by pressing [E]. Or press [F] again to recover it. It looks like your turret sustained some damage during re-entry. When you�re ready to take on the aliens, deploy your turret and repair it back to full health.");
	m_StepsComplete.Add(false);

	Tags.Add("Player");
}

void APlayerMech::Client_DisplaySubtitles_Implementation(const FText& text, float displayLength)
{
	APlayerMechController* controller = Cast<APlayerMechController>(GetController());
	if (!controller) return;

	if (m_SubtitleMenu == nullptr)
	{
		m_SubtitleMenu = CreateWidget(controller, m_SubtitleWidgetClass);
		m_SubtitleMenu->AddToViewport();
	}

	Cast<USubtitleWidget>(m_SubtitleMenu)->ResetSubtitles(FName(*text.ToString()), displayLength);
}

void APlayerMech::Client_PlayDialogue_Implementation(USoundCue* que, bool overridePlayingSound)
{
	if (m_DialogueComponent->IsPlaying())
	{
		if (overridePlayingSound)
			m_DialogueComponent->Stop();
		else
			return;
	}

	m_DialogueComponent->SetSound(que);
	m_DialogueComponent->Play();
}

void APlayerMech::Client_AddDialogueToQue_Implementation(const FDialogue& dialogue)
{
	m_DialogueQue.Add(dialogue);

	if (m_DialogueQue.Num() == 1)
	{
		Client_PlayDialogue(m_DialogueQue[0].m_Cue);
		Client_DisplaySubtitles(m_DialogueQue[0].m_Text);
	}
}

// Called when the game starts or when spawned
void APlayerMech::BeginPlay()
{
	Super::BeginPlay();

	m_CameraArmStartingLocation = m_CameraArm->GetComponentLocation();
	m_OriginalSpawn = GetActorTransform();

	m_CurrentTutorialStep = -1;
	Client_AdvanceTutorial(-1, 5.0f);

	if (!HasAuthority())
		return;
	//Spawn the Turret
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	FTransform baseTransform = FTransform(m_InteractBounds->GetComponentLocation());

	m_Turret = GetWorld()->SpawnActor<ATurret>(m_TurretBlueprint, baseTransform, spawnParams);
	m_Turret->SetOwner(this);
	ATurret_AIController* controller = Cast<ATurret_AIController>(m_Turret->GetController());
	if (controller)
	{
		controller->m_TurretReferance = m_Turret;
		controller->UpdateBehaviorTreeValues(m_Turret);
	}

	m_Turret->NetMulticast_RecoverTurret();
}

void APlayerMech::AddCharacterAbilities()
{
	int numMainWeapons = 0;
	if (HasAuthority())
	{
		//Adding weapons
		FActorSpawnParameters weaponSpawnParams;
		weaponSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		weaponSpawnParams.Owner = this;

		FAttachmentTransformRules transformRules(EAttachmentRule::SnapToTarget, true);

		// For all starting weapons, Spawn the weapon and store in UnlockedWeapons TArray.
		for (int i = 0; i < m_WeaponClasses.Num(); i++)
		{
			m_Weapons.Add(Cast<ABaseWeapon>(GetWorld()->SpawnActor<ABaseWeapon>(m_WeaponClasses[i], GetActorLocation(), FRotator::ZeroRotator, weaponSpawnParams)));
			m_Weapons[i * 2]->AttachToComponent(GetMesh(), transformRules, "Weapon_Right_Socket");
			m_Weapons.Add(Cast<ABaseWeapon>(GetWorld()->SpawnActor<ABaseWeapon>(m_WeaponClasses[i], GetActorLocation(), FRotator::ZeroRotator, weaponSpawnParams)));
			m_Weapons[i * 2 + 1]->AttachToComponent(GetMesh(), transformRules, "Weapon_Left_Socket");
			m_Weapons[i * 2 + 1]->GetSkeletalMeshComponent()->SetRelativeScale3D(FVector(1.0f, -1.0f, 1.0f));
			numMainWeapons += 2;
		}

		for (int i = 0; i < m_SpecialWeaponClasses.Num(); i++)
		{
			m_Weapons.Add(Cast<ABaseWeapon>(GetWorld()->SpawnActor<ABaseWeapon>(m_SpecialWeaponClasses[i], GetActorLocation(), FRotator::ZeroRotator, weaponSpawnParams)));
			m_Weapons[numMainWeapons + i]->AttachToComponent(GetMesh(), transformRules, "Weapon_Special_Socket");
		}
	}

	// If m_EntityAbilitySystemComponent is valid
	if (m_EntityAbilitySystemComponent)
	{
		//Setup default equipped weapons
		m_CurrentRightWeaponIndex = 0;
		m_CurrentLeftWeaponIndex = 1;
		m_CurrentSpecialWeaponIndex = numMainWeapons;

		// If we have Authority and the number of Unlocked Weapons is more than 0
		if (HasAuthority() && m_Weapons.Num() > 0)
		{
			//TODO instead of hardcoded, setup loop. Also add special weapon
			//DON'T FORGET TO UPDATE GE_PlayerInit with correct tags for unlocked weapons!
			m_Weapons[0]->SetActorHiddenInGame(false);
			m_Weapons[1]->SetActorHiddenInGame(false);
			m_Weapons[numMainWeapons]->SetActorHiddenInGame(false);

			//Dodge ability
			m_EntityAbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(m_DodgeClass, 1, 0));
			//Stomp ability
			m_EntityAbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(m_StompClass, 1, 10));
			//LeftFire ability
			m_EntityAbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(m_LeftWeaponFireAbility, 1, 1));
			//RightFire ability
			m_EntityAbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(m_RightWeaponFireAbility, 1, 2));
			//SpecialFire ability
			m_EntityAbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(m_SpecialWeaponFireAbility, 1, 3));
		}
	}
}

void APlayerMech::ReturnToSpawn()
{
	//We only come back to life if the main base isn't dead.
	if (m_MainBase->m_bIsDead == false)
	{
		SetActorTransform(m_OriginalSpawn);
		GetController()->EnableInput(Cast<APlayerController>(GetController()));
		m_bIsDead = false;

		FGameplayEffectContextHandle effectContext = m_EntityAbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpecHandle deathEffectHandle = m_EntityAbilitySystemComponent->MakeOutgoingSpec(m_RespawnEffect, 1, effectContext);
		m_EntityAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*deathEffectHandle.Data.Get());
	}
}

void APlayerMech::Server_PlayerIsReady_Implementation()
{
	AOutpost_ProductionGameMode* gameMode = Cast<AOutpost_ProductionGameMode>(GetWorld()->GetAuthGameMode());
	gameMode->ReadyPlayer(GetPlayerState()->GetPlayerId());
}

void APlayerMech::Client_NextDialogueInQue_Implementation()
{
	m_DialogueQue.RemoveAt(0, 1);
	if (m_DialogueQue.Num() > 0)
	{
		Client_DisplaySubtitles(m_DialogueQue[0].m_Text);
		Client_PlayDialogue(m_DialogueQue[0].m_Cue);
	}
}

void APlayerMech::SelfDestruct()
{
	Server_OnDeath(this);
}

TMap<FString, int> APlayerMech::GenerateWeaponSelectionArray()
{
	TMap<FString, int> availableWeapons;
	for (int i = 0; i < m_Weapons.Num() - m_SpecialWeaponClasses.Num(); i += 2)
	{
		FGameplayTagContainer weaponTagContainer;
		m_Weapons[i]->GetAbilitySystemComponent()->GetOwnedGameplayTags(weaponTagContainer);

		FGameplayTagContainer playerTagContainer;
		GetAbilitySystemComponent()->GetOwnedGameplayTags(playerTagContainer);

		if (playerTagContainer.HasAnyExact(weaponTagContainer))
			availableWeapons.Add(m_Weapons[i]->GetName(), i);
	}

	return availableWeapons;
}

TMap<FString, int> APlayerMech::GenerateSpecialWeaponSelectionArray()
{
	TMap<FString, int> availableSpecial;
	for (int i = m_Weapons.Num() - m_SpecialWeaponClasses.Num(); i < m_Weapons.Num(); i++)
	{
		FGameplayTagContainer weaponTagContainer;
		m_Weapons[i]->GetAbilitySystemComponent()->GetOwnedGameplayTags(weaponTagContainer);

		FGameplayTagContainer playerTagContainer;
		GetAbilitySystemComponent()->GetOwnedGameplayTags(playerTagContainer);

		if (playerTagContainer.HasAnyExact(weaponTagContainer))
			availableSpecial.Add(m_Weapons[i]->GetName(), i);
	}

	return availableSpecial;
}

void APlayerMech::Server_SwitchWeapon_Implementation(int32 index, int32 abilityIDToRemap)
{
	switch (abilityIDToRemap)
	{
	case 1://Left Weapon
	{
		m_Weapons[m_CurrentLeftWeaponIndex]->SetActorHiddenInGame(true);
		m_CurrentLeftWeaponIndex = index;
		m_Weapons[m_CurrentLeftWeaponIndex]->SetActorHiddenInGame(false);
		break;
	}
	case 2://Right Weapon
	{
		m_Weapons[m_CurrentRightWeaponIndex]->SetActorHiddenInGame(true);
		m_CurrentRightWeaponIndex = index;
		m_Weapons[m_CurrentRightWeaponIndex]->SetActorHiddenInGame(false);
		break;
	}
	case 3://Special
	{
		m_Weapons[m_CurrentSpecialWeaponIndex]->SetActorHiddenInGame(true);
		m_CurrentSpecialWeaponIndex = index;
		m_Weapons[m_CurrentSpecialWeaponIndex]->SetActorHiddenInGame(false);
		break;
	}
	}
}

void APlayerMech::OnInteractionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// If the popup screen is already on do nothing.
	if (m_IsInteractionPopupOn == true)
		return;

	// If the overlapping actor is not a interactable do nothing. 
	if (OtherActor->ActorHasTag("Interactable") == false)
		return;

	// If the interactable is a turret but it is not our turret do nothing.
	if (OtherActor->ActorHasTag("Turret") && OtherActor != m_Turret)
		return;

	// If the interactable is the Main Base but it is dead do nothing.
	if (OtherActor->ActorHasTag("MainBase"))
	{
		if (Cast<AMainBase>(OtherActor)->m_bIsDead)
			return;
	}

	// If we are just hitting a objects sight/range sphere do nothing.
	if (OtherComp->ComponentHasTag("Sight"))
		return;

	Client_SpawnPopup(OtherActor);
	m_CurrentPossibleInteract = Cast<AInteractable>(OtherActor);
}

void APlayerMech::EndInteractionOverLap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Check to make sure our popup is on. If not do nothing.
	if (!m_IsInteractionPopupOn)
		return;

	Client_RemovePopup();

	m_CurrentPossibleInteract = nullptr;
}

void APlayerMech::Client_AdvanceTutorial_Implementation(int completedStep, float autoAdvanceTime /*= -1.0f*/)
{
	//Ensure we're only doing this stuff on the local machine
	if (!IsLocallyControlled())
		return;

	//Which ever step we've completed, update our list of completed steps
	if (completedStep >= 0 && completedStep < m_StepsComplete.Num())
		m_StepsComplete[completedStep] = true;

	//If we just completed whatever step we were currently waiting for
	if (completedStep + 1 < m_StepsComplete.Num())
	{
		//If the next step is a valid index
		if (completedStep == m_CurrentTutorialStep)
		{
			m_CurrentTutorialStep = completedStep + 1;
			Client_DisplaySubtitles(FText::FromString(m_TutorialSteps[m_CurrentTutorialStep]), -1.0f);
			GetWorld()->GetTimerManager().ClearTimer(m_AutoAdvanceTimer);

			//If this step has already been completed, set to auto advance after 5 seconds
			if (m_StepsComplete[m_CurrentTutorialStep] == true)
			{
				autoAdvanceTime = 5.0f;
			}

			//If we have an positive autoAdvanceTime, set a timer to advance the tutorial
			if (autoAdvanceTime > 0.0f)
			{
				FTimerDelegate advanceTutDel = FTimerDelegate::CreateUObject(this, &APlayerMech::Client_AdvanceTutorial, m_CurrentTutorialStep, -1.0f);
				GetWorld()->GetTimerManager().SetTimer(m_AutoAdvanceTimer, advanceTutDel, autoAdvanceTime, false);
				UE_LOG(LogTemp, Warning, TEXT("APlayerMech::AdvanceTutorial : auto advance timer set"));
			}
		}
	}
	//Otherwise, we've completed the last part of the tutorial and we should fade out the tutorial text
	else
	{
		m_CurrentTutorialStep = m_TutorialSteps.Num();
		Client_DisplaySubtitles(FText::FromString(m_TutorialSteps.Last()), 1.0f);
		GetWorld()->GetTimerManager().ClearTimer(m_AutoAdvanceTimer);
	}
}

void APlayerMech::Client_RemovePopup_Implementation()
{
	if (m_InteractionMenu)
	{
		m_InteractionMenu->RemoveFromViewport();
		m_IsInteractionPopupOn = false;
	}
}

void APlayerMech::Client_SpawnPopup_Implementation(AActor* OtherActor)
{
	if (!m_InteractionMenu)
	{
		// create menu
		APlayerController* playerController = Cast<APlayerController>(GetController());
		if (!playerController)
		{
			UE_LOG(LogTemp, Warning, TEXT("APlayerMech : OnInteractionOverlapBegin : playerController cast failed"));
			return;
		}
		m_InteractionMenu = CreateWidget(playerController, m_InteractionMenuClass);
	}

	// draw menu
	m_IsInteractionPopupOn = true;

	UTextBlock* descriptionBox = Cast<UTextBlock>(m_InteractionMenu->GetWidgetFromName("DescriptionBox"));
	if (descriptionBox)
	{
		if (OtherActor->ActorHasTag("Turret"))
			descriptionBox->SetText(FText::FromString("Turret"));
		else if (OtherActor->ActorHasTag("MainBase"))
			descriptionBox->SetText(FText::FromString("Main Base"));
		else
			descriptionBox->SetText(FText::FromString("Something"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerMech : OnInteractionOverlapBegin : failed to find menu descriptionBox"));
		return;

	}

	if (!m_InteractionMenu->IsInViewport())
	{
		m_InteractionMenu->AddToViewport();
	}
}

void APlayerMech::Server_Upgrade_Implementation(TSubclassOf<UGameplayEffect> effect, int upgardeCost)
{
	////PLAYER UPGRADE
	UAbilitySystemComponent* playerComponent = GetAbilitySystemComponent();
	bool didUpgrade = false;

	// Check if player abilitycomponent is valid.
	if (!playerComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerMech : Server_Upgrade : UAbilitySystemComponent not valid"));
		//this is a pointless line
		// k
		return;
	}

	FGameplayEffectContextHandle playerEffectContext = playerComponent->MakeEffectContext();

	// If effectContext successfully created
	if (playerEffectContext.IsValid())
	{
		//Set source object
		playerEffectContext.AddSourceObject(this);

		//Apply effect and mark Upgrade as used/purchased
		playerComponent->BP_ApplyGameplayEffectToSelf(effect, 1, playerEffectContext);
		didUpgrade = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerMech : Server_Upgrade : Effect Handle is not valid"));
	}

	////WEAPON UPGRADE
	for (int i = 0; i < GetNumberOfUnlockedWeapons(); i++)
	{
		ABaseWeapon* weapon = GetUnlockedWeaponAtIndex(i);

		if (!weapon)
		{
			UE_LOG(LogTemp, Warning, TEXT("APlayerMech : Server_Upgrade : weapon at index not valid"));
			continue;
		}

		UAbilitySystemComponent* weaponComponent = weapon->GetAbilitySystemComponent();

		//Check if weaponComponent is valid
		if (!weaponComponent)
		{
			UE_LOG(LogTemp, Warning, TEXT("APlayerMech : Server_Upgrade : weaponcomponent not valid"));
			continue;
		}

		FGameplayEffectContextHandle weaponeffectContext = weaponComponent->MakeEffectContext();

		// If effectContext successfully created
		if (weaponeffectContext.IsValid())
		{
			//Set source object
			weaponeffectContext.AddSourceObject(this);

			//Apply effect and mark Upgrade as used/purchased
			weaponComponent->BP_ApplyGameplayEffectToSelf(effect, 1, weaponeffectContext);
			didUpgrade = true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("APlayerMech : Server_Upgrade : weaponEffectContext not valid"));
		}
	}

	////Apply upgrades to turrets.
	if (m_Turret)
	{
		if (m_Turret->ApplyUpgrade(effect))
		{
			didUpgrade = true;
		}
	}

	if (didUpgrade)
	{
		m_Resources -= upgardeCost;
	}
}

ABaseWeapon* APlayerMech::GetUnlockedWeaponAtIndex(int index)
{
	if (m_Weapons.IsValidIndex(index))
	{
		return m_Weapons[index];
	}

	return nullptr;
}


void APlayerMech::Client_SpawnHitMarker_Implementation(float damage, FVector position)
{
	//Checks to ensure this somehow isn't called on the wrong client
	if (!Controller)
		return;

	if (!Controller->IsLocalController())
		return;


	if (m_AvailableWidgets.Num() == 0)
	{
		UHitMarkerWidget* spawnedWidget = Cast<UHitMarkerWidget>(CreateWidget(Cast<APlayerController>(Controller), m_HitMarkerClass));
		if (spawnedWidget)
		{
			spawnedWidget->Init(damage, position);
			spawnedWidget->AddToViewport();
		}
	}
	else
	{
		UHitMarkerWidget* availableWidget = m_AvailableWidgets.Last();
		m_AvailableWidgets.Pop(false);
		availableWidget->Init(damage, position);
		availableWidget->AddToViewport();
	}
}

// Called every frame
void APlayerMech::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerMech::Server_OnDeath_Implementation(AActor* sourceActor)
{
	//Super OnDeath called a the end of the function

	UE_LOG(LogTemp, Warning, TEXT("%s died from %s"), *GetName(), *sourceActor->GetName());

	if (m_bIsDead)
		return;

	//OnEvent_PlayerKilled();

	Cast<AOutpost_ProductionGameMode>(GetWorld()->GetAuthGameMode())->AddToPlayersKilled();
	Super::Server_OnDeath_Implementation(sourceActor);
}

void APlayerMech::Server_BuildTurret_Implementation()
{
	//If the turret exists and is ticking (aka, currently in the world)
	if (m_Turret && m_Turret->IsActorTickEnabled())
		return;

	if (m_TurretBlueprint != nullptr && GetMovementComponent()->IsMovingOnGround())
	{
		FTransform baseTransform = FTransform(m_InteractBounds->GetComponentLocation());
		FVector rayStarts[4] = {};
		rayStarts[0] = baseTransform.GetLocation() + FVector(0.0f, m_TurretRaycastSpread, 0.0f);
		rayStarts[1] = baseTransform.GetLocation() + FVector(0.0f, -m_TurretRaycastSpread, 0.0f);
		rayStarts[2] = baseTransform.GetLocation() + FVector(m_TurretRaycastSpread, 0.0f, 0.0f);
		rayStarts[3] = baseTransform.GetLocation() + FVector(-m_TurretRaycastSpread, 0.0f, 0.0f);


		FHitResult raycast;
		FCollisionQueryParams traceParams;
		traceParams.AddIgnoredActor(this);
		float smallestDistance = TNumericLimits<float>::Max();
		float largestDistance = 0.0f;
		for (int i = 0; i < 4; i++)
		{
			GetWorld()->LineTraceSingleByProfile(raycast, rayStarts[i], rayStarts[i] - FVector(0.0f, 0.0f, m_TurretMaxDistance), FName("BlockAll"), traceParams);
			DrawDebugLine(GetWorld(), rayStarts[i], rayStarts[i] - FVector(0.0f, 0.0f, m_TurretMaxDistance), FColor::Red, false, 3.0f);

			if (raycast.bBlockingHit)
			{
				if (raycast.Distance > largestDistance)
					largestDistance = raycast.Distance;
				else if (raycast.Distance < smallestDistance)
					smallestDistance = raycast.Distance;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("PlayerMech.Server_BuildTurret - ray hit nothing"));
				return;
			}
		}

		if (largestDistance - smallestDistance > m_TurretMaxHeightVariance)
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerMech.Server_BuildTurret - HeighVariance over max"));
			return;
		}

		m_Turret->NetMulticast_BuildTurret(baseTransform);

		m_Turret->SetActorRotation(GetActorForwardVector().ToOrientationRotator(), ETeleportType::None);

		if (m_HasPlacedTurretForFirstTime == false)
		{
			m_HasPlacedTurretForFirstTime = true;
			OnEvent_PlayerPlacedTurret();
		}

	}
}

void APlayerMech::Server_RecoverTurret_Implementation()
{
	m_Turret->NetMulticast_RecoverTurret();
}

void APlayerMech::Server_RepairTurret_Implementation()
{
	const UBaseEntityAttributeSet* turretSet = m_Turret->GetAbilitySystemComponent()->GetSet<UBaseEntityAttributeSet>();

	if (m_Resources >= m_Turret->m_TurretRepairCost &&
		turretSet->Getm_Health() < turretSet->Getm_MaxHealth())
	{
		m_Resources -= m_Turret->m_TurretRepairCost;
		m_Turret->RepairTurret();

		//Check if we've repaired fully and if this is the first time
		if (turretSet->Getm_Health() >= turretSet->Getm_MaxHealth() &&
			m_HasFullyRepairTurretFirstTime == false)
		{
			m_HasFullyRepairTurretFirstTime = true;
			OnEvent_PlayerRepairedTurrets();
		}
	}
}

void APlayerMech::Interact()
{
	if (m_CurrentPossibleInteract != nullptr) m_CurrentPossibleInteract->OpenMenu(this);
}

void APlayerMech::Server_Reload_Implementation()
{
	if (!m_EntityAbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Entity.Dead"))))
	{
		m_Weapons[m_CurrentLeftWeaponIndex]->Reload();
		m_Weapons[m_CurrentRightWeaponIndex]->Reload();

		if (!m_HasReloadedForFirstTime)
		{
			m_HasReloadedForFirstTime = true;
			OnEvent_FirstReload();
		}
	}
}


void APlayerMech::OpenPauseMenu()
{
	APlayerController* playerController = Cast<APlayerController>(GetController());
	if (!m_PauseMenu)
	{
		m_PauseMenu = CreateWidget<UUserWidget>(playerController, m_PauseMenuClass);
		m_IsPauseMenuOpen = true;
	}

	if (m_PauseMenu->IsInViewport())
	{
		m_IsPauseMenuOpen = false;
	}
	else
	{
		m_IsPauseMenuOpen = true;
	}

	m_PauseMenu->AddToViewport();
	playerController->bShowMouseCursor = true;
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(playerController);
	playerController->DisableInput(playerController);
}

void APlayerMech::MoveUp(float value)
{
	if (m_bIsDead == false)
	{
		if ((Controller != NULL) && (value != 0.0f))
		{
			// find out which way is forward
			const FRotator rotation = Controller->GetControlRotation();
			const FRotator yawRotation(0, rotation.Yaw, 0);
			// get forward vector
			const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(direction, value);
		}
	}
}

void APlayerMech::MoveRight(float value)
{
	if (m_bIsDead == false)
	{
		if ((Controller != NULL) && (value != 0.0f))
		{
			// find out which way is right
			const FRotator rotation = Controller->GetControlRotation();
			const FRotator yawRotation(0, rotation.Yaw, 0);
			// get right vector
			const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
			// add movement in that direction
			AddMovementInput(direction, value);
		}
	}
}

void APlayerMech::TurnAtRate(float rate)
{
	if (m_bIsDead == false)
	{
		// calculate delta for this frame from the rate information
		AddControllerYawInput(rate * m_BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void APlayerMech::LookUpRate(float rate)
{
	if (m_bIsDead == false)
	{
		// calculate delta for this frame from the rate information
		AddControllerPitchInput(rate * m_BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

void APlayerMech::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	m_EntityAbilitySystemComponent->RefreshAbilityActorInfo();

	for (ABaseWeapon* weapon : m_Weapons)
	{
		weapon->GetAbilitySystemComponent()->RefreshAbilityActorInfo();
	}
}

void APlayerMech::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	m_EntityAbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent, FGameplayAbilityInputBinds(FString("Confirm"), FString("Cancel"), "AbilityInputID", static_cast<int32>(AbilityInputID::Confirm), static_cast<int32>(AbilityInputID::Cancel)));
}

void APlayerMech::Server_SwitchTurretWeapon_Implementation(int32 index)
{
	m_Turret->SwitchWeapon(index);
}

void APlayerMech::GameWon()
{
	if (!HasAuthority())
		return;

	AOutpost_ProductionGameState* gameState = Cast<AOutpost_ProductionGameState>(GetWorld()->GetGameState());
	if (gameState->m_WaveMaster)
	{
		gameState->m_WaveMaster->ReachedWinCondition();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerMech.GameWon - waveMaster null"));
	}
}

void APlayerMech::GameLost()
{
	if (!HasAuthority())
		return;

	AOutpost_ProductionGameState* gameState = Cast<AOutpost_ProductionGameState>(GetWorld()->GetGameState());
	if (gameState->m_WaveMaster)
	{
		gameState->m_WaveMaster->BeginInfinite();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerMech.GameWon - waveMaster null"));
	}
}

void APlayerMech::Server_SpawnRusher_Implementation(float distance, bool spawnNewIfNeeds, bool testVersion)
{
	FVector direction = m_FollowCamera->GetForwardVector();
	FVector location = GetActorLocation();

	FVector spawnLocation = location + (direction * distance);
	spawnLocation.Z = 600.0f;

	AOutpost_ProductionGameState* gameState = Cast<AOutpost_ProductionGameState>(GetWorld()->GetGameState());

	if (gameState)
	{
		AWaveMaster* waveMaster = gameState->m_WaveMaster;

		if (waveMaster)
		{
			ARusher* rusher = waveMaster->GetAvailableRusher();
			if (rusher)
			{
				rusher->NetMulticast_EnableEnemy(spawnLocation);
				rusher->m_InTestMode = testVersion;
				UE_LOG(LogTemp, Log, TEXT("APlayerMech.SpawnRusher : spawned %s"), *rusher->GetName());
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "PlayerMech.SpawnRusher : spawned " + rusher->GetName());
			}
			else
			{
				if (spawnNewIfNeeds)
				{
					FActorSpawnParameters rusherSpawnParams;
					rusherSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					rusher = GetWorld()->SpawnActor<ARusher>(waveMaster->m_RusherClass, spawnLocation, FRotator::ZeroRotator, rusherSpawnParams);
					if (rusher)
					{
						rusher->NetMulticast_EnableEnemy(spawnLocation);
						rusher->m_InTestMode = testVersion;
						UE_LOG(LogTemp, Log, TEXT("APlayerMech.SpawnRusher : spawned %s"), *rusher->GetName());
						GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "PlayerMech.SpawnRusher : spawned " + rusher->GetName());
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("APlayerMech.SpawnRusher : Failed to get rusher from WaveMaster"));
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("APlayerMech.SpawnRusher : WaveMaster Invalid"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerMech.SpawnRusher : Gamestate Invalid"));
	}
}

void APlayerMech::GodMode()
{
}

void APlayerMech::SetSpeed(float speed)
{
	// TODO : in future, modify value in attribute set to match.
	Cast<UCharacterMovementComponent>(GetMovementComponent())->MaxWalkSpeed = speed;
}

void APlayerMech::Server_Nuke_Implementation()
{
	TArray<AActor*> allEnemies;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Enemy", allEnemies);

	UE_LOG(LogTemp, Warning, TEXT("APlayerMech.Server_Nuke found %d"), allEnemies.Num());

	if (allEnemies.Num() > 0)
	{
		for (auto entity : allEnemies)
		{
			ABaseEnemy* enemy = Cast<ABaseEnemy>(entity);
			if (enemy)
			{
				if (enemy->m_bIsDead == false)
				{
					enemy->Server_OnDeath(this);
				}
			}
		}
	}
}

void APlayerMech::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerMech, m_Turret);
	DOREPLIFETIME(APlayerMech, m_Resources);
	DOREPLIFETIME(APlayerMech, m_Weapons);
	DOREPLIFETIME(APlayerMech, m_MainBase);
	DOREPLIFETIME(APlayerMech, m_CurrentLeftWeaponIndex);
	DOREPLIFETIME(APlayerMech, m_CurrentRightWeaponIndex);
	DOREPLIFETIME(APlayerMech, m_CurrentSpecialWeaponIndex);
}
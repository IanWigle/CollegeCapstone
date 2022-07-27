// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseEntity.h"
#include "DialogueMaster.h"
#include "PlayerMech.generated.h"

UCLASS()
class OUTPOST_PRODUCTION_API APlayerMech : public ABaseEntity
{
    GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turret | Misc")
		TSubclassOf<class ATurret> m_TurretBlueprint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turret | Data")
		float m_TurretRaycastSpread = 30.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turret | Data")
		float m_TurretMaxHeightVariance = 5.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turret | Data")
		float m_TurretMaxDistance = 100.0f;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class AInteractable* m_CurrentPossibleInteract = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Misc")
		class UCapsuleComponent* m_InteractBounds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Misc")
		bool m_DisplayAllyInfo;
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera | Rates")
		float m_BaseTurnRate;
	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera | Rates")
		float m_BaseLookUpRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera | Arm", meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* m_CameraArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera | Camera", meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* m_FollowCamera;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Turret | Menus")
		class UUserWidget* m_TurretMenu;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Upgrade System | Purchased Upgrades")
		TArray<FName> m_PurchasedUpgrades;    
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Misc")
		FTransform m_OriginalSpawn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menus | Pause Menu")
		TSubclassOf<UUserWidget> m_PauseMenuClass;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Menus | Pause Menu")
		UUserWidget* m_PauseMenu;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Menus | Pause Menu")
		bool m_IsPauseMenuOpen = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menus | Interaction Menu")
		TSubclassOf<UUserWidget> m_InteractionMenuClass;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Menus | Interaction Menu")
		UUserWidget* m_InteractionMenu;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Menus | Interaction Menu")
		bool m_IsInteractionPopupOn = false;
    UPROPERTY(BlueprintReadOnly)
        FVector m_CameraArmStartingLocation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menus | Subtitles")
		TSubclassOf<class UUserWidget> m_SubtitleWidgetClass;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Menus | Subtitles")
		UUserWidget* m_SubtitleMenu = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		class UAudioComponent* m_DialogueComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		TArray<FDialogue> m_DialogueQue;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Misc")
		class AMainBase* m_MainBase;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
		TArray<TSubclassOf<class ABaseWeapon>> m_WeaponClasses;
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
		TArray<TSubclassOf<class ABaseWeapon>> m_SpecialWeaponClasses;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Abilities")
		TArray<class ABaseWeapon*> m_Weapons;
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
		TSubclassOf<class UBaseGameplayAbility> m_DodgeClass;
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
		TSubclassOf<UBaseGameplayAbility> m_StompClass;
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
		TSubclassOf<UBaseGameplayAbility> m_LeftWeaponFireAbility;
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
		TSubclassOf<UBaseGameplayAbility> m_RightWeaponFireAbility;
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
		TSubclassOf<UBaseGameplayAbility> m_SpecialWeaponFireAbility;
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
		TSubclassOf<class UGameplayEffect> m_RespawnEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Abilities")
		TSubclassOf<class UHitMarkerWidget> m_HitMarkerClass;
	UPROPERTY(BlueprintReadWrite)
		TArray<UHitMarkerWidget*> m_AvailableWidgets;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Turret |")
		ATurret* m_Turret;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Upgrade System | Resources")
		int m_Resources = 500;
	UPROPERTY(BlueprintReadOnly, Replicated)
		int m_CurrentLeftWeaponIndex;
	UPROPERTY(BlueprintReadOnly, Replicated)
		int m_CurrentRightWeaponIndex;
	UPROPERTY(BlueprintReadOnly, Replicated)
		int m_CurrentSpecialWeaponIndex;
	UPROPERTY(BlueprintReadOnly, Category = "Tutorial")
		bool m_HasPlacedTurretForFirstTime = false;
	UPROPERTY(BlueprintReadWrite, Category = "Tutorial")
		bool m_HasFullyRepairTurretFirstTime = false;
	UPROPERTY(BlueprintReadWrite, Category = "Tutorial")
		bool m_HasFiredLeftWeaponFirstTime = false;
	UPROPERTY(BlueprintReadWrite, Category = "Tutorial")
		bool m_HasFiredRightWeaponFirstTime = false;
	UPROPERTY(BlueprintReadWrite, Category = "Tutorial")
		bool m_HasFiredSpecialWeaponFirstTime = false;
	UPROPERTY(BlueprintReadWrite, Category = "Tutorial")
		bool m_HasReloadedForFirstTime = false;
	UPROPERTY(BlueprintReadWrite, Category = "Tutorial")
		bool m_HasInteractedWithBaseFirstTime = false;

        class AInteractable* m_CurrentInteract = nullptr;

	//Tutorial stuff
	TArray<FString> m_TutorialSteps;
	TArray<bool> m_StepsComplete;

	//Tutorial always starts on Step 1
	int m_CurrentTutorialStep = -1;

	FTimerHandle m_AutoAdvanceTimer;

public:
    // Sets default values for this character's properties
    APlayerMech();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    virtual void Server_OnDeath_Implementation(AActor* sourceActor) override;

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
        void Server_BuildTurret();
    bool Server_BuildTurret_Validate() { return true; }

    UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
        void Server_RecoverTurret();
	bool Server_RecoverTurret_Validate() { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable)
		void Server_RepairTurret();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reload();
		bool Server_Reload_Validate() { return true; }
	void Interact();
	void OpenPauseMenu();

    void MoveUp(float value);
    void MoveRight(float value);
    void TurnAtRate(float rate);
    void LookUpRate(float rate);

	void PossessedBy(AController* NewController) override;
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
		void Server_SwitchWeapon(int32 index, int32 abilityIDToRemap);
		bool Server_SwitchWeapon_Validate(int32 index, int32 abilityIDToRemap) { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
		void Server_SwitchTurretWeapon(int32 index);
		bool Server_SwitchTurretWeapon_Validate(int32 index) { return true; }

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Upgrade(TSubclassOf<UGameplayEffect> effect, int upgardeCost);
		bool Server_Upgrade_Validate(TSubclassOf<UGameplayEffect> effect, int upgardeCost) { return true; }

	UFUNCTION(BlueprintCallable)
		ABaseWeapon* GetUnlockedWeaponAtIndex(int index);
	UFUNCTION(BlueprintCallable)
		int GetNumberOfUnlockedWeapons() { return m_Weapons.Num(); }
	UFUNCTION(BlueprintCallable)
		int GetResources() { return m_Resources; }
	UFUNCTION(BlueprintCallable, Exec)
		void AddResources(int resources) {
		m_Resources += resources;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "PlayerMech.AddResources - m_Resources: " + FString::FromInt(m_Resources));
	}

	UFUNCTION(BlueprintCallable)
		ATurret* GetTurret() {return m_Turret;}

    UFUNCTION(BlueprintImplementableEvent)
        void DisplayWinWidget();

	UFUNCTION(BlueprintCallable, Client, Reliable)
		void Client_SpawnHitMarker(float damage, FVector position);

	TArray<TSubclassOf<class ABaseWeapon>> GetMainWeaponClasses() { return m_WeaponClasses; }
	TArray<TSubclassOf<class ABaseWeapon>> GetSpecialWeaponClasses() { return m_SpecialWeaponClasses; }

    UFUNCTION(NetMulticast, WithValidation, Reliable)
        void NetMulticast_DisplayWin();
        bool NetMulticast_DisplayWin_Validate() { return true; }
        void NetMulticast_DisplayWin_Implementation() { DisplayWinWidget(); }

	UFUNCTION(Client, Reliable)
		void Client_DisplaySubtitles(const FText& text, float displayLength = 5.0f);

	UFUNCTION(Client, Reliable)
		void Client_PlayDialogue(class USoundCue* que, bool overridePlayingSound = false);

	UFUNCTION(Client, Reliable)
		void Client_AddDialogueToQue(const FDialogue& dialogue);

	UFUNCTION(BlueprintCallable)
		void ReturnToSpawn();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    virtual void AddCharacterAbilities() override;

	// Called when the player reaches OnDeath() (Do not implement in .cpp)
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void OnEvent_PlayerKilled();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void OnEvent_PlayerPlacedTurret();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void OnEvent_PlayerRepairedTurrets();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void OnEvent_PlayerFiredLeftWeapon();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void OnEvent_PlayerFiredRightWeapon();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void OnEvent_PlayerFiredSpecial();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void OnEvent_FirstReload();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void OnEvent_FirstBaseInteract();
	
	UFUNCTION(BlueprintCallable)
		TMap<FString, int> GenerateWeaponSelectionArray();
	UFUNCTION(BlueprintCallable)
		TMap<FString, int> GenerateSpecialWeaponSelectionArray();

	UFUNCTION()
		void OnInteractionOverlapBegin(UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	UFUNCTION()
		void EndInteractionOverLap(UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	UFUNCTION(Client, Reliable)
		void Client_SpawnPopup(AActor* OtherActor);

	UFUNCTION(Client, Reliable)
		void Client_RemovePopup();

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable)
		void Server_PlayerIsReady();
		bool Server_PlayerIsReady_Validate() { return true; }

	UFUNCTION(Client, Reliable)
		void Client_NextDialogueInQue();

	UFUNCTION(Client, Reliable, BlueprintCallable)
		void Client_AdvanceTutorial(int completedStep, float autoAdvanceTime = -1.0f);

private:
	UFUNCTION(Exec)
		void SelfDestruct();
    UFUNCTION(Exec)
        void GameWon();
	UFUNCTION(Exec)
		void GameLost();
	UFUNCTION(Exec)
		void SpawnRusher(float distance, bool spawnNewIfNeeds, bool testVersion = true) {Server_SpawnRusher(distance,spawnNewIfNeeds, testVersion);}
	UFUNCTION(Exec)
		void GodMode();
	UFUNCTION(Exec)
		void SetSpeed(float speed);
	UFUNCTION(Exec)
		void NukeEverything() { Server_Nuke(); }

	UFUNCTION(Server, WithValidation, Reliable)
		void Server_Nuke();
		bool Server_Nuke_Validate() { return true; }
	UFUNCTION(Server, WithValidation, Reliable)
		void Server_SpawnRusher(float distance, bool spawnNewIfNeeds, bool testVersion = true);
		bool Server_SpawnRusher_Validate(float distance, bool spawnNewIfNeeds, bool testVersion = true) { return true; }
};

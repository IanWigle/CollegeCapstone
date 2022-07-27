#pragma once

#include "CoreMinimal.h"

#define ACTOR_ROLE_FSTRING *(FindObject<UEnum>(ANY_PACKAGE, TEXT("ENetRole"), true)->GetNameStringByValue(Role))
#define GET_ACTOR_ROLE_FSTRING(Actor) *(FindObject<UEnum>(ANY_PACKAGE, TEXT("ENetRole"), true)->GetNameStringByValue(Actor->Role))

UENUM(BlueprintType)
enum class AbilityInputID : uint8
{
    //0 None
    Dodge				 UMETA(DisplayName = "Dodge"),
    //1 Confirm
    LeftFire			 UMETA(DisplayName = "LeftFire"),
    //2 Cancel
    RightFire		     UMETA(DisplayName = "RightFire"),
    //3 LMB/Left Trigger
    SpecialFire			 UMETA(DisplayName = "SpecialFire"),
    //4 RMB/Right Trigger
    Interact		     UMETA(DisplayName = "Interact"),
    //5 
    Reload				 UMETA(DisplayName = "Reload"),
    //6
    Turret				 UMETA(DisplayName = "Turret"),
    //7
    OpenMenu			 UMETA(DisplayName = "OpenMenu"),
	//8
	Confirm              UMETA(DisplayName = "Confirm"),
	//9
	Cancel				 UMETA(DisplayName = "Cancel"),
	//10
	Stomp				 UMETA(DisplayName = "Stomp")
};

UENUM(BlueprintType)
enum class DialogueEnum : uint8
{
	Debrief1					UMETA(DisplayName = "Debrief 1"),
	Debrief2					UMETA(DisplayName = "Debrief 2"),
	TurretDeployed				UMETA(DisplayName = "Turret Deployed"),
	BaseQuarterDamage			UMETA(DisplayName = "Base 3/4"),
	BaseHalfDamage				UMETA(DisplayName = "Base 1/2"),
	BaseFinalQuarterDamage		UMETA(DisplayName = "Base 1/4"),
	BaseDestroyed				UMETA(DisplayName = "Base Destroyed"),
	AllPlayersKilled			UMETA(DisplayName = "All Players Killed"),
	GameWon						UMETA(DisplayName = "Game Won"),
	PilotKilled					UMETA(DisplayName = "Pilot Killed"),
	PilotRespawned				UMETA(DisplayName = "Pilot Respawned"),
	WaveStarting				UMETA(DisplayName = "Wave Starting"),
	WaveEnding					UMETA(DisplayName = "Wave Ending"),
	SecondObjExplained			UMETA(DisplayName = "Secondary Objective Explained"),
	SecondObjSent				UMETA(DisplayName = "Secondary Objective Sent"),
	SecondObjComplete			UMETA(DisplayName = "Secondary Objective Complete"),
	SecondObjFailed				UMETA(DisplayName = "Secondary Objective Failed")
};
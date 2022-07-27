// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums.h"
#include "DialogueMaster.generated.h"

USTRUCT(BlueprintType)
struct FDialogue
{
	GENERATED_BODY()
public:
	FDialogue(FText text, class USoundCue* cue)
	{
		m_Text = text;
		m_Cue = cue;
	}

	FDialogue() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText m_Text;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundCue* m_Cue;
};

USTRUCT(BlueprintType)
struct FDialogueList
{
	GENERATED_BODY()
public:
	FDialogueList()
	{

	}

	FDialogue GetRandomVariant()
	{
		return m_Varients[FMath::RandHelper(m_Varients.Num())];
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FDialogue> m_Varients;
};

UCLASS()
class OUTPOST_PRODUCTION_API ADialogueMaster : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADialogueMaster();

	UFUNCTION(BlueprintCallable, Server, WithValidation, Reliable)
		void Server_PlayDialogueForAll(DialogueEnum choice, bool overrideAlreadyPlaying = false);
		bool Server_PlayDialogueForAll_Validate(DialogueEnum choice, bool overrideAlreadyPlaying = false) { return true; };

	UFUNCTION(BlueprintCallable, Server, WithValidation, Reliable)
		void Server_PlayDialogueForClient(DialogueEnum choice, class APlayerMech* player, bool overrideAlreadyPlaying = false);
		bool Server_PlayDialogueForClient_Validate(DialogueEnum choice, class APlayerMech* player, bool overrideAlreadyPlaying = false) { return true; }

protected:
	void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TMap<DialogueEnum, FDialogueList> m_DialogueOptions;
};

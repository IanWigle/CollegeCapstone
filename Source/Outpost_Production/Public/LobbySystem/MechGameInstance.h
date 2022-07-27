// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PlayerMech.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionInterface.h"
#include "MechGameInstance.generated.h"

/*****************************************************************************/
/* This struct is used to collect and store needed data for a session/server */
/*****************************************************************************/
USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUsername;
};

/************************************************************************/
/* This struct stores all data related to a mech varient                */
/************************************************************************/
USTRUCT(BlueprintType)
struct FMechVariant
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NotReplicated)
		FString VariantName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, NotReplicated)
		TSubclassOf<class APlayerMech> VarientMech;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, NotReplicated)
		class UDataTable* Table;
};

/**************************************************************************/
/* This class is responcible for handliing of Lobby Setup, the Player's   */
/* mech customization, and saving option menus that handles game settings */ 
/**************************************************************************/
UCLASS(Config = Game)
class OUTPOST_PRODUCTION_API UMechGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMechGameInstance(const FObjectInitializer& ObjectInitializer);

	virtual void Init();

	UFUNCTION(BlueprintCallable, Exec)
		void Host(FString serverName);

	UFUNCTION(BlueprintCallable)
		void SelectIndex(int32 index);

	UFUNCTION(BlueprintCallable)
		void Join();

	UFUNCTION(BlueprintCallable, Exec)
		void StopHosting();

	void PopulateServerWidget(TArray<FServerData> serverEntryList);

	void StartSession();

	UFUNCTION(BlueprintCallable)
		void RefreshServerList(UUserWidget* serverListWidget);

	UFUNCTION(BlueprintCallable)
		void SavePlayersChoice(int userIndex, FMechVariant varient);

	UFUNCTION(BlueprintCallable)
		int GetControllerNetPlayerIndex(class APlayerController* controller);

private:

	void OnCreateSessionComplete(FName sessionName, bool success);
	void OnDestroySessionComplete(FName sessionName, bool success);
	void OnFindSessionComplete(bool success);
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);

	void CreateSession();
	void EndSession();

public:
#pragma region Lobby Setup
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby | Setup Data")
		FName m_SessionName = TEXT("Game");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby | Setup Data")
		FName m_ServerNameSettingsKey = TEXT("ServerName");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Lobby | Setup Data")
		bool m_IsLan = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Lobby | Setup Data")
		bool m_UseTestLevel = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby | Setup Data")
		int32 m_MaxSearchResults = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Lobby | Setup Data")
		int32 m_NumberOfConnections = 4;
	UPROPERTY(BlueprintReadWrite, Config, Category = "Lobby | Setup Data")
		int32 m_MaxNumberOfPlayers = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby | Setup Data")
		bool m_ShouldAdvertise = true;
#pragma endregion Lobby Setup
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menus")
        TSubclassOf<class UUserWidget> m_OptionsMenuWidget;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menus")
        UUserWidget* m_OptionsMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Customization | Varients")
		TArray<FMechVariant> m_VariousChoices;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Customization | Players Choices")
		TMap<int, FMechVariant> m_PlayersChoices;

protected:

	TSubclassOf<class UUserWidget> m_MenuClass;

	TSubclassOf<class UUserWidget> m_InGameMenuClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lobby | Server List")
		TSubclassOf<class UUserWidget> m_ServerListEntryClass;

	int32 m_SelectedServerListIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby | Server List")
		UUserWidget* m_ServerListWidget;

	UPROPERTY(EditAnywhere)
		FString m_LobbyReferance;
private:

	IOnlineSessionPtr m_SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> m_SessionSearch;

	UFUNCTION(Exec)
		void UseDebugLevel(bool state) { m_UseTestLevel = state; }
};

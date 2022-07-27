// Fill out your copyright notice in the Description page of Project Settings.


#include "MechGameInstance.h"
#include "Engine/Engine.h"
#include "UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "LobbySystem/ServerRow.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "OnlineSessionSettings.h"
#include "Components/AudioComponent.h"

//const static FName STATIC_SESSION_NAME = TEXT("Game");
//const static FName m_ServerNameSettingsKey = TEXT("ServerName");

UMechGameInstance::UMechGameInstance(const FObjectInitializer& ObjectInitializer)
{
	//ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/")
	m_ServerListWidget = nullptr;
}

void UMechGameInstance::Init()
{
	Super::Init();

	IOnlineSubsystem* subsystem = IOnlineSubsystem::Get();

	if (subsystem != nullptr)
	{
		m_SessionInterface = subsystem->GetSessionInterface();

		if (m_SessionInterface.IsValid())
		{
			// SUBSCRIBE to the OnCreateSessionCompeleteDelegates
			m_SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMechGameInstance::OnCreateSessionComplete);
			// SUBSCRIBE to the OnDestroySessionCompeleteDelegates
			m_SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMechGameInstance::OnDestroySessionComplete);
			// SUBSCRIBE to the OnFindSessionsCompeleteDelegates
			m_SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMechGameInstance::OnFindSessionComplete);
			// SUBSCRIBE to the OnJoinSessionCompeleteDelegates
			m_SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMechGameInstance::OnJoinSessionComplete);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Found no subsystem"));
	}
}

void UMechGameInstance::Join()
{
	if (!m_SessionInterface.IsValid()) return;
	if (!m_SessionSearch.IsValid()) return;

	m_SessionInterface->JoinSession(0, m_SessionName, m_SessionSearch->SearchResults[m_SelectedServerListIndex]);
}

void UMechGameInstance::StartSession()
{
	if (m_SessionInterface.IsValid())
	{
		m_SessionInterface->StartSession(m_SessionName);
	}
}

void UMechGameInstance::Host(FString serverName)
{
	//ASSIGN DesiredServerName to ServerName
	//IF SessionInterface.IsValid()
	if (m_SessionInterface.IsValid())
	{
		/*Check if there is a existing session and destroy it*/
		//DECLARE a auto variable called ExistingSession and assign it to the return value of SessionInterface->GetNamedSession(SESSION_NAME)
		auto existingSession = m_SessionInterface->GetNamedSession(m_SessionName);
		//IF ExistingSession NOT equal to nullptr
		if (existingSession != nullptr)
		{
			/*THis will trigger the OnDestroySessionComplete(...) which will then call CreateSession()*/
			//CALL DestroySession on SessionInterface and pass in SESSION_NAME
			m_SessionInterface->DestroySession(m_SessionName);
		}
		//ELSE
		else
		{
			//CALL CreateSession()
			CreateSession();
		}
		//ENDIF
	}
	//ENDIF
}

void UMechGameInstance::SelectIndex(int32 Index)
{
	m_SelectedServerListIndex = Index;
}

void UMechGameInstance::RefreshServerList(UUserWidget* serverListWidget)
{
	m_SessionSearch = MakeShareable(new FOnlineSessionSearch());

	if (m_SessionSearch.IsValid())
	{
		m_SessionSearch->bIsLanQuery = m_IsLan;

		m_SessionSearch->MaxSearchResults = m_MaxSearchResults;

		m_SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

		m_SessionInterface->FindSessions(0, m_SessionSearch.ToSharedRef());

		m_ServerListWidget = serverListWidget;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, "UMechGameInstance::RefreshServerList - SessionSearch Invalid");
	}
}

void UMechGameInstance::SavePlayersChoice(int userIndex, FMechVariant varient)
{
	m_PlayersChoices.Add(userIndex, varient);
}

int UMechGameInstance::GetControllerNetPlayerIndex(APlayerController* controller)
{
	return controller->NetPlayerIndex;
}

void UMechGameInstance::StopHosting()
{
	EndSession();
}

void UMechGameInstance::PopulateServerWidget(TArray<FServerData> serverEntryList)
{
	UWorld* world = this->GetWorld();
	if (!ensure(world != nullptr)) return;

	UScrollBox* serverListScrollBox = Cast<UScrollBox>(m_ServerListWidget->GetWidgetFromName("ServerListScrollBox"));
	if (serverListScrollBox == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMechGameInstance.PopulateServerWidget - UScrollBox not found"));
		return;
	}

	serverListScrollBox->ClearChildren();

	uint32 i = 0;
	for (const FServerData& ServerData : serverEntryList)
	{
		UServerRow* Row = CreateWidget<UServerRow>(world, m_ServerListEntryClass);
		if (!ensure(Row != nullptr)) return;

		Row->m_ServerName->SetText(FText::FromString(ServerData.Name));
		Row->m_HostUser->SetText(FText::FromString(ServerData.HostUsername));
		FString FractionText = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);
		Row->m_ConnectionFraction->SetText(FText::FromString(FractionText));
		Row->Setup(this, i);
		++i;

		serverListScrollBox->AddChild(Row);
	}
}

void UMechGameInstance::OnCreateSessionComplete(FName sessionName, bool success)
{
	if (!success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not create session"));
		return;
	}

	
	//ENDIF

	//DECLARE a variable callled Engine of type UEngine* and assign it to the return of GetEngine()
	UEngine* engine = GetEngine();
	if (!ensure(engine != nullptr)) return;

	engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

	//DECLARE a variable called World of type UWorld* and assign it to the return value of GetWorld()
	UWorld* world = GetWorld();

	if (!ensure(world != nullptr)) return;

	//World'/GameFrwkSessionsPlugin/Maps/Lobby.Lobby'
	/*Open the Lobby Level in listen mode*/
	//CALL ServerTravel() on the World and pass in the following string "/GameFrwkSessionsPlugin/Maps/Lobby?listen"
	world->ServerTravel(m_LobbyReferance + "?listen");
}

void UMechGameInstance::OnDestroySessionComplete(FName sessionName, bool success)
{
	
}

void UMechGameInstance::OnFindSessionComplete(bool success)
{
	//IF Success AND SessionSearch.IsValid() AND Menu NOT EQUAL nullptr
	if (success && m_SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Finished Find Session"));
		//DECLARE a TArray of type FServerData called ServerNames
		TArray<FServerData> serverNames;
		/*Loop through the found sessions and create server data for each found session*/
		//FOR ( const FOnlineSessionSearchResult& SearchResult IN SessionSearch->SearchResults )
		for (const FOnlineSessionSearchResult& searchResult : m_SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found session names: %s"), *searchResult.GetSessionIdStr());
			//DECLARE a variable called Data of type FServerData
			FServerData Data;
			//ASSIGN MaxPlayers in Data to SearchResult.Session.SessionSettings.NumPublicConnections
			Data.MaxPlayers = searchResult.Session.SessionSettings.NumPublicConnections;
			//ASSIGN CurrentPlayers in Data to Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections 
			Data.CurrentPlayers = Data.MaxPlayers - searchResult.Session.NumOpenPublicConnections;
			//ASSIGN HostUsername in Data to SearchResult.Session.OwningUserName
			Data.HostUsername = searchResult.Session.OwningUserName;

			//DECLARE a FString called ServerName
			FString serverName;
			//IF SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName) /*Gets the Server name from the Session's Settings*/
			if (searchResult.Session.SessionSettings.Get(m_ServerNameSettingsKey, serverName))
			{
				//ASSIGN Name in Data to ServerName
				Data.Name = serverName;
			}
			//ELSE
			else
			{
				//ASSIGN Name in Data to "Could not find name."
				Data.Name = "Could not find name.";
				
			}
			//ENDIF

			//CALL Add in ServerNames and pass in Data
			serverNames.Add(Data);
		}
		//ENDFOR

		PopulateServerWidget(serverNames);

	}
	//ENDIF
}

void UMechGameInstance::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	if (!m_SessionInterface.IsValid()) return;

	FString address;
	/*Look at the documentation for GetResolvedConnectString() or go into OnlineSessionInterface.h and look up this function*/
	if (!m_SessionInterface->GetResolvedConnectString(sessionName, address)) {
		UE_LOG(LogTemp, Warning, TEXT("Could not get connect string."));
		return;
	}

	UEngine* engine = GetEngine();
	if (!ensure(engine != nullptr)) return;

	engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Joining %s"), *address));

	APlayerController* playerController = GetFirstLocalPlayerController();
	if (!ensure(playerController != nullptr)) return;

	/*This will transfer the client to the network address of where the session is being hosted*/
	//CALL ClientTravel() on the PlayerController and pass in Address, ETravelType::TRAVEL_Absolute
	playerController->ClientTravel(address, ETravelType::TRAVEL_Absolute);
}

void UMechGameInstance::CreateSession()
{
	//IF SessionInterface.IsValid()
	if (m_SessionInterface.IsValid())
	{
		//DECLARE a variable called SessionSettings of type FOnlineSessionSettings
		FOnlineSessionSettings sessionSettings;
		//DECLARE a variable called name and assign it to the return value of IOnlineSubsystem::Get()->GetSubsystemName()
		FName name = IOnlineSubsystem::Get()->GetSubsystemName();
		/*We will always be in a lan match, this will be something else if the subsystem is steam, XBox Live, PSN ....*/
		//IF the name.IsEqual("NULL")
		if (name.IsEqual("NULL"))
		{
			//ASSIGN bIsLANMatch in SessionSettings to true
			sessionSettings.bIsLANMatch = true;
		}
		//ELSE
		else
		{
			//ASSIGN bIsLANMatch in SessionSettings to false
			sessionSettings.bIsLANMatch = false;
		}
		//ENDIF
		//ASSIGN NumPublicConnections in SessionSettings to 5 /** The number of publicly available connections advertised */
		sessionSettings.NumPublicConnections = m_NumberOfConnections;
		//ASSIGN bShouldAdvertise in SessionSettings to true /**this will advertise whether this match is advertized on the online service*/
		sessionSettings.bShouldAdvertise = m_ShouldAdvertise;
		//ASSIGN bUsesPresence in SessionSettings to true /** Whether to display user presence information or not */
		sessionSettings.bUsesPresence = true;
		//CALL Set on SessionSettings and pass in SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing
		sessionSettings.Set(m_ServerNameSettingsKey, m_SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		//CALL CreateSession() on the SessionInterface and pass in 0, SESSION_NAME, SessionSettings
		m_SessionInterface->CreateSession(0, m_SessionName, sessionSettings);
	}
	//ENDIF
}

void UMechGameInstance::EndSession()
{
	if (m_SessionInterface.IsValid())
	{
		m_SessionInterface->EndSession(m_SessionName);
	}
}
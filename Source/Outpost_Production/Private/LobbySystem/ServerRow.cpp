// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerRow.h"
#include "..\..\Public\LobbySystem\ServerRow.h"
#include "Components/Button.h"
#include "MechGameInstance.h"

void UServerRow::Setup(UMechGameInstance* gameInstance, uint32 InIndex)
{
	m_GameInstance = gameInstance;
	Index = InIndex;
	RowButton->OnClicked.AddDynamic(this, &UServerRow::OnClicked);
}

void UServerRow::OnClicked()
{
	m_GameInstance->SelectIndex(Index);
}

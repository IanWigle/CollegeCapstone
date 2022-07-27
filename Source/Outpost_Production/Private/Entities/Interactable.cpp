// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"
#include "PlayerMech.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

AInteractable::AInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);

	Tags.Add("Interactable");
}

void AInteractable::OpenMenu(APlayerMech* player)
{
	APlayerController* playerController = Cast<APlayerController>(player->GetController());
	if (!playerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("AInteractable.OpenMenu - playerController cast failed"));
	}
	
	if (!m_Menu)
	{
		m_Menu = CreateWidget<UUserWidget>(playerController, m_MenuClass);
	}

	m_Menu->AddToViewport();
	playerController->bShowMouseCursor = true;
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(playerController);
	playerController->DisableInput(playerController);
}

void AInteractable::CloseMenu(APlayerMech* player)
{
	APlayerController* playerController = Cast<APlayerController>(player->GetController());
	if (!playerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("AInteractable.OpenMenu - playerController cast failed"));
	}

	playerController->bShowMouseCursor = false;
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(playerController);
	playerController->EnableInput(playerController);
}

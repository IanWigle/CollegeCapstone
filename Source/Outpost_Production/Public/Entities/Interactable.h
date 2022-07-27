// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "Interactable.generated.h"

/**
 * 
 */
UCLASS()
class OUTPOST_PRODUCTION_API AInteractable : public ABaseEntity
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
		TSubclassOf<class UUserWidget> m_MenuClass;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Menu")
		UUserWidget* m_Menu = nullptr;

public:
	AInteractable();

	UFUNCTION(BlueprintCallable)
		void OpenMenu(APlayerMech* player);

	UFUNCTION(BlueprintCallable)
		void CloseMenu(APlayerMech* player);

};

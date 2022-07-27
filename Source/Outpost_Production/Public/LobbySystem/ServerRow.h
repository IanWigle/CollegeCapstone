// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRow.generated.h"

/**
 * 
 */
UCLASS()
class OUTPOST_PRODUCTION_API UServerRow : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* m_ServerName;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* m_HostUser;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* m_ConnectionFraction;

	UPROPERTY(BlueprintReadOnly)
		bool m_Selected = false;

public:
	void Setup(class UMechGameInstance* InParent, uint32 InIndex);

private:
	UPROPERTY(meta = (BindWidget))
		class UButton* RowButton;

	UPROPERTY()
		class UMechGameInstance* m_GameInstance;

	uint32 Index;

	UFUNCTION(BlueprintCallable)
		void OnClicked();
};

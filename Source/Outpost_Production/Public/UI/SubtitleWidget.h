// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SubtitleWidget.generated.h"

/**
 * 
 */
UCLASS()
class OUTPOST_PRODUCTION_API USubtitleWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BLueprintReadWrite, meta = (BindWidget))
		class UTextBlock* Description;

public:

	UFUNCTION(BlueprintImplementableEvent)
		void ResetSubtitles(FName text, float length);
};

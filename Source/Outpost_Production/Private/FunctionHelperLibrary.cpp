// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionHelperLibrary.h"
#include "BaseEntity.h"
#include "Kismet/GameplayStatics.h"

AActor* UFunctionHelperLibrary::GetClosestObjectToSourceByFilter(UWorld* world, AActor* sourceObj, TSubclassOf<AActor> filter)
{
	TArray<AActor*> foundActors;

	UGameplayStatics::GetAllActorsOfClass(Cast<const UObject>(world), filter, foundActors);

	if (foundActors.Num() > 0)
	{
		float lowestDistance = TNumericLimits<float>::Max();
		int lowestDistanceIndex = 0;

		for (int i = 0; i < foundActors.Num(); i++)
		{
			float newLosDistance = FVector::Dist(sourceObj->GetActorLocation(), foundActors[i]->GetActorLocation());
			if (newLosDistance < lowestDistance)
			{
				lowestDistance = newLosDistance;
				lowestDistanceIndex = i;
			}
		}

		if (lowestDistance != TNumericLimits<float>::Max())
		{
			return foundActors[lowestDistanceIndex];
		}
	} 
	else
	{
		return nullptr;
	}

	return nullptr;
}

AActor* UFunctionHelperLibrary::GetClosestObjectToSourceByTag(UWorld* world, AActor* sourceObj, FName tag)
{
	TArray<AActor*> foundActors;

	UGameplayStatics::GetAllActorsWithTag(Cast<const UObject>(world), tag, foundActors);

	if (foundActors.Num() > 0)
	{
		float lowestDistance = TNumericLimits<float>::Max();
		int lowestDistanceIndex = 0;

		for (int i = 0; i < foundActors.Num(); i++)
		{
			float newLosDistance = FVector::Dist(sourceObj->GetActorLocation(), foundActors[i]->GetActorLocation());
			if (newLosDistance < lowestDistance)
			{
				lowestDistance = newLosDistance;
				lowestDistanceIndex = i;
			}
		}

		if (lowestDistance != TNumericLimits<float>::Max())
		{
			return foundActors[lowestDistanceIndex];
		}
	}
	else
	{
		return nullptr;
	}

	return nullptr;
}

AActor* UFunctionHelperLibrary::GetClosestObjectToSourceByFilterPlusTag(UWorld* world, AActor* sourceObj, TSubclassOf<AActor> filter, FName tag)
{
	TArray<AActor*> foundActors;

	UGameplayStatics::GetAllActorsOfClassWithTag(Cast<const UObject>(world), filter, tag, foundActors);

	if (foundActors.Num() > 0)
	{
		float lowestDistance = TNumericLimits<float>::Max();
		int lowestDistanceIndex = 0;

		for (int i = 0; i < foundActors.Num(); i++)
		{
			float newLosDistance = FVector::Dist(sourceObj->GetActorLocation(), foundActors[i]->GetActorLocation());
			if (newLosDistance < lowestDistance)
			{
				lowestDistance = newLosDistance;
				lowestDistanceIndex = i;
			}
		}

		if (lowestDistance != TNumericLimits<float>::Max())
		{
			return foundActors[lowestDistanceIndex];
		}
	}
	else
	{
		return nullptr;
	}

	return nullptr;
}

//void UFunctionHelperLibrary::NetMulticast_DisableEntity_Implementation(class ABaseEntity* entity)
//{
//	//sets actor to not appear in game and disables their tick and collision
//	entity->SetActorHiddenInGame(true);
//	entity->SetActorTickEnabled(false);
//	entity->SetActorEnableCollision(false);
//	entity->GetCharacterMovement()->GravityScale = 0.0f;
//	entity->m_bIsDead = true;
//}
//
//void UFunctionHelperLibrary::NetMulticast_EnableEntity_Implementation(ABaseEntity* entity, FVector newLocation)
//{
//	//sets actor to appear in the game and enables their tick and collision
//	entity->SetActorHiddenInGame(false);
//	entity->SetActorTickEnabled(true);
//	entity->SetActorEnableCollision(true);
//	entity->GetCharacterMovement()->GravityScale = 1.0f;
//	entity->SetActorLocation(newLocation, false, nullptr, ETeleportType::TeleportPhysics);
//	entity->m_bIsDead = false;
//}
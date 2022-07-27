// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseFloraClass.h"
#include "..\..\Public\Flora\BaseFloraClass.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UnrealNetwork.h"

// Sets default values
ABaseFloraClass::ABaseFloraClass()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_FloraMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Flora Mesh");
	RootComponent = m_FloraMesh;

	m_ProximitySphere = CreateDefaultSubobject<USphereComponent>("Proximity Sphere");
	m_ProximitySphere->SetupAttachment(RootComponent);
	m_ProximitySphere->SetCanEverAffectNavigation(false);
	m_ProximitySphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseFloraClass::OnOverlapBegin);
	m_ProximitySphere->OnComponentEndOverlap.AddDynamic(this, &ABaseFloraClass::EndOverLap);

	SetReplicates(true);
}

void ABaseFloraClass::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp != OtherActor->GetRootComponent())
		return;

	if (!OtherActor->ActorHasTag("Entity"))
		return;

	m_EnteredActors.Add(OtherActor);

	ActorEntersProximity(OtherActor);
}

void ABaseFloraClass::EndOverLap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherComp != OtherActor->GetRootComponent())
		return;

	if (m_EnteredActors.Contains(OtherActor))
		m_EnteredActors.Remove(OtherActor);

	ActorLeavesProximity(OtherActor);
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT_Service_BaseEnemyAggro.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "BaseEnemy.h"

void UBT_Service_BaseEnemyAggro::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ABaseEnemy* owningEnemy =  Cast<ABaseEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	UBlackboardComponent* owningBlackboard = OwnerComp.GetBlackboardComponent();

	if (owningEnemy && owningBlackboard)
	{
		owningEnemy->DetermineAggro(0.0f, nullptr);
		owningBlackboard->SetValueAsObject("Target", owningEnemy->GetCurrentTarget());
	}
}

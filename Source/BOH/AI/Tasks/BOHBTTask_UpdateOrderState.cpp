// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHBTTask_UpdateOrderState.h"

// UnrealEngine
#include "BehaviorTree/BlackboardComponent.h"

// BOH
#include "BOH/AI/BOHAIController.h"

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

UBOHBTTask_UpdateOrderState::UBOHBTTask_UpdateOrderState()
{
	NodeName = "Update Order";
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

EBTNodeResult::Type UBOHBTTask_UpdateOrderState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	ABOHAIController* UnitAIController = BlackboardComponent ? Cast<ABOHAIController>(OwnerComp.GetAIOwner()) : nullptr;
	FUnitOrder NewOrder;
	bool bValidNextOrder = UnitAIController->GetNextOrder(NewOrder);
	if (!bValidNextOrder)
	{
		return EBTNodeResult::Failed;
	}
	
	BlackboardComponent->SetValueAsEnum(GetCurrentOrderStateBlackboardKey(), static_cast<uint8>(NewOrderState));
	UnitAIController->UpdateOrderState(NewOrderState);
	
	EBTNodeResult::Type Result = EBTNodeResult::Succeeded;
	FinishLatentTask(OwnerComp, Result);
	return Result;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "UBOHBTTask_WaitUntilNewOrder.h"

// UnrealEngine
#include "BehaviorTree/BlackboardComponent.h"

// BOH
#include "BOH/AI/BOHAIController.h"

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

UBOHBTTask_WaitUntilNewOrder::UBOHBTTask_WaitUntilNewOrder()
{
	
	NodeName = "Wait Until New Order";
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

EBTNodeResult::Type UBOHBTTask_WaitUntilNewOrder::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	ABOHAIController* UnitAIController = BlackboardComponent ? Cast<ABOHAIController>(OwnerComp.GetAIOwner()) : nullptr;
	FUnitOrder NewOrder;
	bool bValidNextOrder = UnitAIController->GetNextOrder(NewOrder);
	if (!bValidNextOrder)
	{
		BlackboardComponent->SetValueAsEnum(GetCurrentOrderTypeBlackboardKey(), static_cast<uint8>(EUnitOrderType::None));
		return EBTNodeResult::Failed;
	}
	
	BlackboardComponent->SetValueAsEnum(GetCurrentOrderTypeBlackboardKey(), static_cast<uint8>(NewOrder.OrderType));

	EBTNodeResult::Type Result = EBTNodeResult::Succeeded;
	FinishLatentTask(OwnerComp, Result);
	return Result;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

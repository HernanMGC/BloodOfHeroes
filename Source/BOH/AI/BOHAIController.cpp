// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHAIController.h"

// UnrealEngine
#include "BehaviorTree/BehaviorTree.h"

// BOH
#include "BOH/Characters/BOHUnit.h"

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHAIController::SendUnitOrder(FBOHUnitOrder NewUnitOrder)
{
	NewUnitOrder.OrderState = EUnitOrderState::Queued;
	if (!NewUnitOrder.IsValid())
	{
		return;
	}
	
	if (UnitOrders.IsEmpty())
	{
		UnitOrders.Add(NewUnitOrder);
		return;
	}
	
	FBOHUnitOrder& CurrentUnitOrder = UnitOrders[0];

	switch (NewUnitOrder.OrderSortingPolicy)
	{
	case EUnitOrderSortingPolicy::MAX:
	case EUnitOrderSortingPolicy::None:
		break;
	case EUnitOrderSortingPolicy::AddToQueue:
		UnitOrders.Add(NewUnitOrder);
		break;
	case EUnitOrderSortingPolicy::AddAfterCurrent:
		UnitOrders.EmplaceAt(1);
		break;
	case EUnitOrderSortingPolicy::InterruptCurrentMissable:
		if (!CurrentUnitOrder.bCanBeInterrupted)
		{
			return;
		}
	case EUnitOrderSortingPolicy::InterruptCurrentQueueable:
		UnitOrders.EmplaceAt(1, NewUnitOrder);
		if (CurrentUnitOrder.bCanBeInterrupted)
		{
			UpdateOrderState(EUnitOrderState::Finished);
			bInterruptedDirty = true;
			OnOrderInterrupted.Broadcast();
		}
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

bool ABOHAIController::GetNextOrder(FBOHUnitOrder& NextOrder) const
{
	if (UnitOrders.Num() > 0)
	{
		NextOrder = UnitOrders[0];
	}

	return !UnitOrders.IsEmpty();
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHAIController::UpdateOrderState(EUnitOrderState InOrderState)
{
	if (UnitOrders.Num() <= 0)
	{
		return;
	}

	if (InOrderState == EUnitOrderState::Finished)
	{
		ConsumeOrder();
		return;
	}
	UnitOrders[0].OrderState = InOrderState;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHAIController::ConsumeOrder()
{
	if (UnitOrders.Num() > 0)
	{
		UnitOrders.RemoveAt(0);
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ABOHUnit* Unit = InPawn ? Cast<ABOHUnit>(InPawn) : nullptr;
	UBehaviorTree* BehaviorTree = Unit ? Unit->GetBehaviorTree() : nullptr;
	if (!BehaviorTree)
	{
		return;
	}

	UBlackboardComponent* BBComp = nullptr;
	UseBlackboard(BehaviorTree->BlackboardAsset, BBComp);
	Blackboard = BBComp;
	CurrentBehaviorTree = BehaviorTree;
	RunBehaviorTree(CurrentBehaviorTree);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

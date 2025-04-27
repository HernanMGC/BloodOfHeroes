// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHBTService_CatchOrderInterruptions.h"

// BOH
#include "BehaviorTree/BlackboardComponent.h"
#include "BOH/AI/BOHAIController.h"

//// ForwardDeclarations
// BOH
class ABOHAIController;

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

UBOHBTService_CatchOrderInterruptions::UBOHBTService_CatchOrderInterruptions()
{
	NodeName = "Catch Order Interruptions";
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHBTService_CatchOrderInterruptions::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ABOHAIController* UnitAIController = Cast<ABOHAIController>(OwnerComp.GetAIOwner());
	bool bHasBeenInterrupted = UnitAIController ? UnitAIController->bInterruptedDirty : false;
	UBlackboardComponent* BlackboardComponent = bHasBeenInterrupted ? OwnerComp.GetBlackboardComponent() : nullptr;
	if (!BlackboardComponent)
	{
		return;
	}

	UnitAIController->bInterruptedDirty = false;
	BlackboardComponent->SetValueAsEnum(GetCurrentOrderStateBlackboardKey(), static_cast<uint8>(EUnitOrderState::Finished));
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHBTService_CatchOrderInterruptions::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	// BTComp = OwnerComp;
	// ABOHAIController* UnitAIController = BTComp ? Cast<ABOHAIController>(BTComp->GetAIOwner()) : nullptr;
	// if (!UnitAIController)
	// {
	// 	return;
	// }
	// UnitAIController->OnOrderInterrupted.AddUniqueDynamic(this, &ThisClass::OnOrderInterrupted);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHBTService_CatchOrderInterruptions::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// if (ABOHAIController* UnitAIController = BTComp ? Cast<ABOHAIController>(OwnerComp.GetAIOwner()) : nullptr)
	// {
	// 	UnitAIController->OnOrderInterrupted.RemoveDynamic(this, &ThisClass::OnOrderInterrupted);
	// }
	//
	// Super::OnCeaseRelevant(OwnerComp, NodeMemory);
}

void UBOHBTService_CatchOrderInterruptions::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	// Super::OnSearchStart(SearchData);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHBTService_CatchOrderInterruptions::OnOrderInterrupted()
{}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

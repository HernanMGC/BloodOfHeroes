// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHBTTask_FindNextPathLocation.h"

// UnrealEngine
#include "BehaviorTree/BlackboardComponent.h"

// BOH
#include "BOH/AI/BOHAIController.h"
#include "BOH/Characters/BOHUnit.h"
#include "BOH/Component/Path/BOHUnitPathComponent.h"

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

UBOHBTTask_FindNextPathLocation::UBOHBTTask_FindNextPathLocation()
{
	NodeName = "Find Next Path Target Location";
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

EBTNodeResult::Type UBOHBTTask_FindNextPathLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return EBTNodeResult::Failed;
	}

	const ABOHAIController* UnitAIController = Cast<ABOHAIController>(OwnerComp.GetAIOwner());
	const ABOHUnit* Unit = UnitAIController ? Cast<ABOHUnit>(UnitAIController->GetPawn()) : nullptr;
	const UBOHUnitPathComponent* PathComponent = Unit ? Unit->GetComponentByClass<UBOHUnitPathComponent>() : nullptr;
	FPathTargetLocation NextPathPoint = FPathTargetLocation();
	const int32 NextPathPointIndex = BlackboardComponent->GetValueAsInt(GetTargetPositionIndexBlackboardKey()) + 1;
	bool ValidPathPoint = PathComponent
		                      ? PathComponent->FindPathPointAtIndex(NextPathPointIndex, NextPathPoint)
		                      : false;
	if (!ValidPathPoint)
	{
		return EBTNodeResult::Failed;
	}

	BlackboardComponent->SetValueAsVector(GetTargetPositionBlackboardKey(), NextPathPoint.RealPathTargetLocation);
	const double RemainingDistanceToTargetLoc = (NextPathPoint.PathTargetLocation - NextPathPoint.RealPathTargetLocation).
		Length();
	BlackboardComponent->SetValueAsBool(GetTargetLocationReachedBlackboardKey(),
	                                    FMath::IsNearlyZero(RemainingDistanceToTargetLoc));
	BlackboardComponent->SetValueAsInt(GetTargetPositionIndexBlackboardKey(), NextPathPointIndex);

	constexpr EBTNodeResult::Type Result = EBTNodeResult::Succeeded;
	FinishLatentTask(OwnerComp, Result);
	return Result;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

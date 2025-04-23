// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHBTTask_FindNextPathLocation.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BloodOfHeroes/AI/BOHAIController.h"
#include "BloodOfHeroes/Characters/BOHCharacter.h"
#include "BloodOfHeroes/Component/Path/BOHUnitPathComponent.h"

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
	
	ABOHAIController* UnitAIController = Cast<ABOHAIController>(OwnerComp.GetAIOwner());
	ABOHCharacter* Unit = UnitAIController ? Cast<ABOHCharacter>(UnitAIController->GetPawn()) : nullptr;
	UBOHUnitPathComponent* PathComponent = Unit ? Unit->GetComponentByClass<UBOHUnitPathComponent>() : nullptr;
	FVector NextPathPoint = FVector::ZeroVector;
	int32 NextPathPointIndex = BlackboardComponent->GetValueAsInt(GetTargetPositionIndexBlackboardKey()) + 1;
	bool ValidPathPoint = PathComponent ? PathComponent->FindPathPointAtIndex(NextPathPointIndex, NextPathPoint) : false;
	if (!ValidPathPoint)
	{
		return EBTNodeResult::Failed;
	}
	
	BlackboardComponent->SetValueAsVector(GetTargetPositionBlackboardKey(), NextPathPoint);
	BlackboardComponent->SetValueAsInt(GetTargetPositionIndexBlackboardKey(), NextPathPointIndex);
	EBTNodeResult::Type Result = EBTNodeResult::Succeeded;
	FinishLatentTask(OwnerComp, Result);
	return Result;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

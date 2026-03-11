// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHBTTask_UpdatePathForNextTurn.h"

// UnrealEngine
#include "BehaviorTree/BlackboardComponent.h"

// BOH
#include "BOH/AI/BOHAIController.h"
#include "BOH/Characters/BOHUnit.h"
#include "BOH/Component/Path/BOHPathActor.h"
#include "BOH/Component/Path/BOHUnitPathComponent.h"
#include "BOH/Messages/BOHGameplayMessage.h"
#include "BOH/Tags/BOHGameplayTagCollection.h"
#include "BOH/Utils/BOHUtils.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Kismet/GameplayStatics.h"

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

DEFINE_LOG_CATEGORY(LogBOHTask_UpdatePathForNextTurn);

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

UBOHBTTask_UpdatePathForNextTurn::UBOHBTTask_UpdatePathForNextTurn()
{
	NodeName = "Update Path For Next Turn";
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

EBTNodeResult::Type UBOHBTTask_UpdatePathForNextTurn::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return EBTNodeResult::Failed;
	}

	const ABOHAIController* UnitAIController = Cast<ABOHAIController>(OwnerComp.GetAIOwner());
	ABOHUnit* Unit = UnitAIController ? Cast<ABOHUnit>(UnitAIController->GetPawn()) : nullptr;
	UBOHUnitPathComponent* PathComponent = Unit ? Unit->GetComponentByClass<UBOHUnitPathComponent>() : nullptr;
	if (!PathComponent)
	{
		return EBTNodeResult::Failed;
	}

	int32 LastPathPointIndex = BlackboardComponent->GetValueAsInt(GetTargetPositionIndexBlackboardKey()) - 1;
	const bool LastPathPointIndexReached = BlackboardComponent->GetValueAsBool(GetTargetLocationReachedBlackboardKey());
	if (LastPathPointIndexReached) { LastPathPointIndex++; }

	const bool Interrupted = BlackboardComponent->GetValueAsBool(GetOrderInterruptedBlackboardKey());
	if (Interrupted) { LastPathPointIndex--; }

	for (int32 i = 0; i <= LastPathPointIndex; i++)
	{
		PathComponent->RemovePointFromPath(0);
	}

	PathComponent->AddPointToPath(Unit->GetActorLocation(), 0);
	BOH_LOG(LogBOHTask_UpdatePathForNextTurn, Display, "Client_SetUnitPath");
	
	// TODO: Check whether this can be done in a less coupled way
	UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(this);
	FBOHUnitPath UnitPathUpdate;
	UnitPathUpdate.UnitPtr = Unit;
	UnitPathUpdate.UnitPath = PathComponent->GetUnitPath();

	FBOHUnitPathEndedMessage UnitPathEnded;
	UnitPathEnded.UniPath = UnitPathUpdate;
	UnitPathEnded.UnitTurnEndWorldTimeInSeconds = UGameplayStatics::GetTimeSeconds(this);
	
	GameplayMessageSubsystem.BroadcastMessage(UBOHGameplayTagCollection::Get().Tag_MessageChannel_UnitPathEnded, UnitPathEnded);

	BlackboardComponent->SetValueAsBool(GetTargetLocationReachedBlackboardKey(), false);
	BlackboardComponent->SetValueAsInt(GetTargetPositionIndexBlackboardKey(), 0);
	
	constexpr EBTNodeResult::Type Result = EBTNodeResult::Succeeded;
	FinishLatentTask(OwnerComp, Result);
	return Result;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

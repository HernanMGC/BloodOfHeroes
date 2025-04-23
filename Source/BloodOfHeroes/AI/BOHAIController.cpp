// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHAIController.h"

// UnrealEngine
#include "BehaviorTree/BehaviorTree.h"

// BOH
#include "BloodOfHeroes/Characters/BOHCharacter.h"

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHAIController::RunCurrentBehaviorTree()
{
	RunBehaviorTree(CurrentBehaviorTree);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ABOHCharacter* Unit = InPawn ? Cast<ABOHCharacter>(InPawn) : nullptr;
	UBehaviorTree* BehaviorTree = Unit ? Unit->GetBehaviorTree() : nullptr;
	if (!BehaviorTree)
	{
		return;
	}

	UBlackboardComponent* BBComp = nullptr;
	UseBlackboard(BehaviorTree->BlackboardAsset, BBComp);
	Blackboard = BBComp;
	// RunBehaviorTree(BehaviorTree);
	CurrentBehaviorTree = BehaviorTree;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

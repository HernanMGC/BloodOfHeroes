// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"

// BOH
#include "BOHBTTask_UpdatePathForNextTurn.generated.h"

/**
 * Update path component for next turn.
 */
UCLASS()
class BLOODOFHEROES_API UBOHBTTask_UpdatePathForNextTurn : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	/**
	 * Constructor. Sets Node name and other basic setup.
	 */
	UBOHBTTask_UpdatePathForNextTurn();
	
	// Overriden to: Update agent path component for next turn.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	/**
	 * Returns target position index BB key.
	 * @return 
	 */
	FORCEINLINE FName GetTargetPositionIndexBlackboardKey() const
	{
		return TargetPositionIndexBlackboardKey.SelectedKeyName;
	}
	
	/**
	 * Returns target location reached BB key.
	 * @return 
	 */
	FORCEINLINE FName GetTargetLocationReachedBlackboardKey() const
	{
		return TargetLocationReachedBlackboardKey.SelectedKeyName;
	}
	
protected:
	// Target position index BB key.
	UPROPERTY(EditAnywhere, Category="BOH|Blackboard")
	FBlackboardKeySelector TargetPositionIndexBlackboardKey;
	
	// Target position BB key.
	UPROPERTY(EditAnywhere, Category="BOH|Blackboard")
	FBlackboardKeySelector TargetLocationReachedBlackboardKey;
};

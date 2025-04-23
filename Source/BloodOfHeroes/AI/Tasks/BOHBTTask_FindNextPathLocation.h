// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BOHBTTask_FindNextPathLocation.generated.h"

/**
 * Find next position. 
 */
UCLASS()
class BLOODOFHEROES_API UBOHBTTask_FindNextPathLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	/**
	 * Constructor. Sets Node name and other basic setup.
	 */
	UBOHBTTask_FindNextPathLocation();
	
	// Overriden to: Perform search next path target location from agent UBOHUnitPathComponent and update blackboard.
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
	 * Returns target position BB key.
	 * @return 
	 */
	FORCEINLINE FName GetTargetPositionBlackboardKey() const
	{
		return TargetPositionBlackboardKey.SelectedKeyName;
	}
	
protected:
	// Target position index BB key.
	UPROPERTY(EditAnywhere, Category="BOH|Blackboard")
	FBlackboardKeySelector TargetPositionIndexBlackboardKey;
	
	// Target position BB key.
	UPROPERTY(EditAnywhere, Category="BOH|Blackboard")
	FBlackboardKeySelector TargetPositionBlackboardKey;
};

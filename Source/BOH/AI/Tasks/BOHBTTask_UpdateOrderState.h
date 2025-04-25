// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"

// BOH
#include "BOH/AI/BOHUnitOrders.h"
#include "BOHBTTask_UpdateOrderState.generated.h"

/**
 * Consumes order and sets the BB to consume next order.
 */
UCLASS()
class BOH_API UBOHBTTask_UpdateOrderState : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	// New order state.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUnitOrderState NewOrderState = EUnitOrderState::None;
	
public:
	/**
	 * Constructor. Sets Node name and other basic setup.
	 */
	UBOHBTTask_UpdateOrderState();
	
	// Overriden to: Consumes order and resets BB.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	/**
	 * Returns current order state BB key.
	 * @return 
	 */
	FORCEINLINE FName GetCurrentOrderStateBlackboardKey() const
	{
		return CurrentOrderStateBlackboardKey.SelectedKeyName;
	}
	
protected:
	// Current order state index BB key.
	UPROPERTY(EditAnywhere, Category="BOH|Blackboard")
	FBlackboardKeySelector CurrentOrderStateBlackboardKey;
};

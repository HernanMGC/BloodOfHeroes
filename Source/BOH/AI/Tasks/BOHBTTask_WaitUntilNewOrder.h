// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"

// BOH
#include "BOHBTTask_WaitUntilNewOrder.generated.h"

/**
 * Wait until there is a new order.
 */
UCLASS()
class BOH_API UBOHBTTask_WaitUntilNewOrder : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	/**
	 * Constructor. Sets Node name and other basic setup.
	 */
	UBOHBTTask_WaitUntilNewOrder();
	
	// Overriden to: Wait until new order and setup order for controller.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	/**
	 * Returns current order type BB key.
	 * @return 
	 */
	FORCEINLINE FName GetCurrentOrderTypeBlackboardKey() const
	{
		return CurrentOrderTypeBlackboardKey.SelectedKeyName;
	}
	
	/**
	 * Returns current order state BB key.
	 * @return 
	 */
	FORCEINLINE FName GetCurrentOrderStateBlackboardKey() const
	{
		return CurrentOrderStateBlackboardKey.SelectedKeyName;
	}
	
	/**
	 * Returns order interrupted BB key.
	 * @return 
	 */
	FORCEINLINE FName GetOrderInterruptedBlackboardKey() const
	{
		return OrderInterruptedBlackboardKey.SelectedKeyName;
	}
	
protected:
	// Current order type index BB key.
	UPROPERTY(EditAnywhere, Category="BOH|Blackboard")
	FBlackboardKeySelector CurrentOrderTypeBlackboardKey;

	// Current order state index BB key.
	UPROPERTY(EditAnywhere, Category="BOH|Blackboard")
	FBlackboardKeySelector CurrentOrderStateBlackboardKey;
	
	// Order interrupted BB key.
	UPROPERTY(EditAnywhere, Category="BOH|Blackboard")
	FBlackboardKeySelector OrderInterruptedBlackboardKey;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// UnrealEngine
// Includes
#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"

// BOH
#include "BOHUnitOrders.h"
#include "BOHAIController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOrderInterrupted);

/**
 * Base class for Unit AI controller.
 */
UCLASS(Abstract)
class BOH_API ABOHAIController : public AAIController
{
	GENERATED_BODY()
public:
	// On order interrupted delegate.
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnOrderInterrupted OnOrderInterrupted;

	// Order has been interrupted and service must check it.
	bool bInterruptedDirty = false;

public:
	/**
	 * Send unit order.
	 * @param NewUnitOrder 
	 */
	UFUNCTION(BlueprintCallable)
	void SendUnitOrder(FUnitOrder NewUnitOrder);

	/**
	 * Returns true if there is a next order, and return the oder through NextOrder reference. NextOrder is  the first
	 * of the list.
	 * @param NextOrder Output value for NextOrder.
	 * @return 
	 */
	bool GetNextOrder(OUT FUnitOrder& NextOrder) const;

	/**
	 * Update next order state.
	 * @param InOrderState 
	 */
	void UpdateOrderState(EUnitOrderState InOrderState);
	
	/**
	 * Consumes first order if any.
	 */
	void ConsumeOrder();
	
protected:
	// Overriden to: Setup behavior tree.
	virtual void OnPossess(APawn* InPawn) override;

protected:
	// Current behavior tree.
	UPROPERTY(Transient)
	UBehaviorTree* CurrentBehaviorTree = nullptr;

	// List of unit orders. It's a FIFO list.
	UPROPERTY(Transient)
	TArray<FUnitOrder> UnitOrders;
};

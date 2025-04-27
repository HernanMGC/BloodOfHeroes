// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"

// BOH
#include "BOHBTService_CatchOrderInterruptions.generated.h"

/**
 * Behavior Tree service for catching order interruptions.
 */
UCLASS()
class BOH_API UBOHBTService_CatchOrderInterruptions : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	/**
	 * Constructor. Sets Node name and other basic setup.
	 */
	UBOHBTService_CatchOrderInterruptions();

protected:
	// Overriden to catch
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData) override;
	
	UFUNCTION()
	void OnOrderInterrupted();

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
	// Current order state index BB key.
	UPROPERTY(EditAnywhere, Category="BOH|Blackboard")
	FBlackboardKeySelector CurrentOrderStateBlackboardKey;
	
	// Order interrupted BB key.
	UPROPERTY(EditAnywhere, Category="BOH|Blackboard")
	FBlackboardKeySelector OrderInterruptedBlackboardKey;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// UnrealEngine
// Includes
#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"

// BOH
#include "BOHAIController.generated.h"

/**
 * Base class for Unit AI controller.
 */
UCLASS(Abstract)
class BLOODOFHEROES_API ABOHAIController : public AAIController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void RunCurrentBehaviorTree();
	
protected:
	// Overriden to: Setup behavior tree.
	virtual void OnPossess(APawn* InPawn) override;
	
protected:
	// Current behavior tree.
	UPROPERTY(Transient)
	UBehaviorTree* CurrentBehaviorTree = nullptr;
};

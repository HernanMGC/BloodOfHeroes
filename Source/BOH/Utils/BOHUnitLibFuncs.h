// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

// BOH
#include "BOHUnitLibFuncs.generated.h"

//// ForwardDeclaration
// BOH
class ABOHUnit;

/**
 * BP Lib funcs for units.
 */
UCLASS()
class BOH_API UBOHUnitLibFuncs : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Returns max distance for unit in a turn.
	 * @return 
	 */
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContext"))
	static float GetMaxDistanceForUnitPerTurn(const UObject* WorldContext, ABOHUnit* Unit);
};

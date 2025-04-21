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
class ABOHCharacter;

/**
 * BP Lib funcs for characters.
 */
UCLASS()
class BLOODOFHEROES_API UBOHUnitLibFuncs : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Returns max distance for character in a turn.
	 * @return 
	 */
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContext"))
	static float GetMaxDistanceForCharacter(const UObject* WorldContext, ABOHCharacter* Unit);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"

// BOH
#include "BOHPathActor.h"
#include "BOHPathLineActor.generated.h"

/**
 * Base class for Path Lines.
 */
UCLASS(Abstract)
class BLOODOFHEROES_API ABOHPathLineActor : public ABOHPathActor
{
	GENERATED_BODY()

public:
	/**
	 * Sets line light.
	 * @param LineLength
	 * @param TotalLineLength 
	 */
	void SetLineLength(float LineLength, float TotalLineLength);

	/**
	 * Function called on changes on line length. 
	 * @param LineLength
	 * @param TotalLineLength
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnLineLengthChanged(float LineLength, float TotalLineLength);
};

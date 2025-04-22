// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

// BOH
#include "BOHGameModeBase.generated.h"

/**
 * Baase game mode for Blood of Heroes.
 */
UCLASS()
class BLOODOFHEROES_API ABOHGameModeBase : public AGameMode
{
	GENERATED_BODY()

public:
	/**
	 * Returns turn time in seconds.
	 * @return 
	 */
	FORCEINLINE float GetTurnTime() const { return TurnTime; }
	
protected:
	// Turn time in seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BOH|Turn", meta = (Units = "s"))
	float TurnTime = 3.f;
};

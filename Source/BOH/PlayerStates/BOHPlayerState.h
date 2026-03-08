// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BOHPlayerState.generated.h"

UENUM(BlueprintType)
enum class EBOHPlayerTurnState : uint8
{
	None UMETA(DisplayName = "None"),
	Planning UMETA(DisplayName = "Planning"),
	Waiting UMETA(DisplayName = "Waiting"),
	Resolving UMETA(DisplayName = "Resolving"),
	MAX UMETA(Hidden)
};

/**
 * Base class for player state.
 */
UCLASS(Abstract)
class BOH_API ABOHPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	// Overriden to: send a message on players name update.
	virtual void OnRep_PlayerName() override;

	/**
	 * On rep sends s message for player score update.
	 */
	void OnRep_PlayerScore(); 

protected:
	// Player score
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 PlayerScore = 0;

	// Current turn state
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	EBOHPlayerTurnState TurnState = EBOHPlayerTurnState::None;
};

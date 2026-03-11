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

public:
	ABOHPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * Sets player score.
	 * @param InTeamScore 
	 */
	void SetTeamScore(int32 InTeamScore);

	/**
	 * Returns player score.
	 * @return 
	 */
	FORCEINLINE int32 GetTeamScore() const { return TeamScore; };;
	
protected:
	// Overriden to: send a message on players name update.
	virtual void OnRep_PlayerName() override;

	/**
	 * On rep sends s message for player score update.
	 */
	UFUNCTION()
	void OnRep_TeamScore(); 

protected:
	// Player score. This is an int32 version of the player score in APlayerState.
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_TeamScore, VisibleAnywhere, BlueprintReadOnly)
	int32 TeamScore = INDEX_NONE;

	// Current turn state
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	EBOHPlayerTurnState TurnState = EBOHPlayerTurnState::None;
};

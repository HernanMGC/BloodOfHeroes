// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "GameFramework/GameState.h"

// BOH
#include "BOHGameState.generated.h"

/**
 * This class handlers player turns.
 */
UCLASS()
class BOH_API ABOHGameState : public AGameState
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void OnRep_MatchTotalTime();
	
	UFUNCTION()
	void OnRep_MatchCurrentTime();

	/**
	 * Sets player score.
	 * @param InMatchCurrentTime 
	 */
	void SetMatchCurrentTime(float InMatchCurrentTime);

	/**
	 * Returns player score.
	 * @return 
	 */
	FORCEINLINE float GetMatchCurrentTime() const { return MatchCurrentTime; }
	
	/**
	 * Sets player score.
	 * @param InMatchTotalTime 
	 */
	void SetMatchTotalTime(float InMatchTotalTime);

	/**
	 * Returns player score.
	 * @return 
	 */
	FORCEINLINE float GetMatchTotalTime() const { return MatchTotalTime; }
	
	/**
	 * Sets player score.
	 * @param InMaxTimePerTurn 
	 */
	FORCEINLINE void SetMaxTimePerTurn(float InMaxTimePerTurn);

	/**
	 * Returns player score.
	 * @return 
	 */
	FORCEINLINE float GetMaxTimePerTurn() const { return MaxTimePerTurn; }
	
protected:
	// Overriden to: setup match time (total and current). 
	virtual void HandleMatchIsWaitingToStart() override;
	
protected:
	// Turn time in seconds
	UPROPERTY(ReplicatedUsing = OnRep_MatchTotalTime, VisibleAnywhere, BlueprintReadOnly, Category = "BOH|Turn", meta = (Units = "s", ClampMin = 0.f))
	float MatchTotalTime = 0.f;

	// Turn time in seconds
	UPROPERTY(ReplicatedUsing = OnRep_MatchCurrentTime, VisibleAnywhere, BlueprintReadOnly, Category = "BOH|Turn", meta = (Units = "s", ClampMin = 0.f))
	float MatchCurrentTime = 0.f;

	// Time per turn in seconds
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "BOH|Turn", meta = (Units = "s", ClampMin = 0.f))
	float MaxTimePerTurn = 0.f;
};

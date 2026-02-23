// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

// BOH
#include "BOHGameModeBase.generated.h"

class ABOHUnit;
/**
 * Base game mode for Blood of Heroes.
 */
UCLASS(Abstract)
class BOH_API ABOHGameModeBase : public AGameMode
{
	GENERATED_BODY()

public:
	/**
	 * Returns turn time in seconds.
	 * @return 
	 */
	FORCEINLINE float GetTurnTime() const { return TurnTime; }

	/**
	 * Returns default unit class.
	 * @return 
	 */
	FORCEINLINE TSubclassOf<ABOHUnit> GetDefaultUnitClass() const { return DefaultUnitClass; }
	

protected:
	// Overriden to: Prevent two player to start from the same player start.
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	// Overriden to: spawn units per controller.
	virtual void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;
	
protected:
	// Turn time in seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BOH|Turn", meta = (Units = "s"))
	float TurnTime = 3.f;

	// Default unit class.
	UPROPERTY(EditAnywhere, Category = "BOH|Classes")
	TSubclassOf<ABOHUnit> DefaultUnitClass = nullptr;
	
	// List of occupied player starts as only one player per PlayerStart is allowed.
	UPROPERTY(Transient)
	TArray<APlayerStart*> OccupiedPlayerStarts;
};

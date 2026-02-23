// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"

// BOH
#include "BOHPlayerStart.generated.h"

/**
* Class that handles player start, i.e.: the spawn point for the camera pawn and the player's units.
*/
UCLASS()
class BOH_API ABOHPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	/**
	 * Sets default values for this actor's properties
	 */
	ABOHPlayerStart(const FObjectInitializer& ObjectInitializer);

	// Overriden to: initialize UnitSpawnPoints length and their default positions.
	virtual void PostLoad() override;

	/**
	 * Returns UnitSpawnPoints relative to the player start.
	 * @return 
	 */
	FORCEINLINE TArray<FVector> GetUnitSpawnPointsRelativeLocations() const { return UnitSpawnPoints; }

	/**
	 * Returns UnitSpawnPoints transform in world coordinates.
	 * @return 
	 */
	FORCEINLINE TArray<FTransform> GetUnitSpawnPointsTransformWorld() const;

protected:
	/**
	 * Function that creates a instance button for resetting unit spawn points.
	 */
	UFUNCTION(CallInEditor)
	void ResetUnitSpawnPoints();

	/**
	 * Create UnitPerPlayerStart points and initializes as equidistant points along a line on the Y-axis centered at
	 * actor's location.
	 */
	void InitializeUnitSpawnPoints();
	
protected:
	// Unit spawn points.
	UPROPERTY(EditInstanceOnly, EditFixedSize, meta = (MakeEditWidget))
	TArray<FVector> UnitSpawnPoints;

	// TODO: Maybe this needs to be something saved in Project Settings or GameMode
	UPROPERTY(EditInstanceOnly, meta = (Units = "cm", ClampMin = 0.f))
	float StartingLineMaxDistance = 1000.f;

	// TODO: Maybe this needs to be something saved in Project Settings or GameMode
	// Units to spawn per player start. Used at PostLoad to initialize UnitSpawnPoints on first load.
	int32 UnitPerPlayerStart = 5;
};

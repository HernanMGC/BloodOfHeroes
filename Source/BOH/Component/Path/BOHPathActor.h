// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// BOH
#include "BOHPathActor.generated.h"

//// ForwardDeclarations
// UnrealEngine
class UStaticMeshComponent;

// BOH
class ABOHUnit;

DECLARE_LOG_CATEGORY_EXTERN(LogBOHPathActor, Log, All);

/**
 * Unit move commands.
 */
USTRUCT(BlueprintType)
struct FBOHUnitPath
{
	GENERATED_BODY()

public:
	// Unit ID.
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABOHUnit> UnitPtr = nullptr;

	// Ordered list of unit intended moves.
	UPROPERTY(BlueprintReadOnly)
	TArray<FVector> UnitMoves;

public:
	/**
	 * Default constructor. It constructs an invalid FBOHUnitPath
	 */
	FBOHUnitPath();

	/**
	 * Explicit constructor.
	 * @param InUnitPtr 
	 * @param InUnitMoves 
	 */
	FBOHUnitPath(ABOHUnit* InUnitPtr, TArray<FVector> InUnitMoves);

	/**
	 * Equal operator for UnitPath.
	 * @return 
	 */
	bool operator==(const FBOHUnitPath& Other) const;
	
	/**
	 * Checks if Unit path has valid info.
	 * @return 
	 */
	bool IsValid() const;

	/**
	 * Return human friend Unit info to print.
	 * @return 
	 */
	FString ToString() const;
};

/**
 * Base class for path actor.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class BOH_API ABOHPathActor : public AActor
{
	GENERATED_BODY()

public:
	// Static mesh component.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent = nullptr;
	
	// Actor size in meters.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BOH|PathLine", meta = (ClampMin = 0.f, Units = "meters"))
	float ActorSize = .3f;

public:
	/**
	 *  Constructor. Removes tick.
	 */
	ABOHPathActor();

	/**
	 * Return owner unit.
	 * @return 
	 */
	UFUNCTION(BlueprintCallable)
	ABOHUnit* GetOwnerUnit();

	// Overriden to: Set actor size on BP construction.
	virtual void OnConstruction(const FTransform& Transform) override;

	/**
	 * Get Path point index.
	 * @return 
	 */
	FORCEINLINE int32 GetPathPointIndex() const { return PathPointIndex; };

	/**
	 * Set Path Point index.
	 * @param InPathPointIndex 
	 */
	FORCEINLINE void SetPathPointIndex(int32 InPathPointIndex){ PathPointIndex = InPathPointIndex; };

	/**
	 * Can actor be edited by controller.
	 * @return 
	 */
	FORCEINLINE bool CanBeEdit() const { return bCanBeEdit; };

	/**
	 * Set if actor can be edited by controller.
	 * @return 
	 */
	FORCEINLINE void SetCanBeEdit(bool bInCanBeEdit) { bCanBeEdit = bInCanBeEdit; };

	/**
	 * Is actor selected by controller.
	 * @return 
	 */
	FORCEINLINE bool IsPathActorSelected() const { return bIsPathActorSelected; };

	/**
	 * Sets if actor has been selected by controller.
	 * @return 
	 */
	void SetIsPathActorSelected(bool bInIsPathActorSelected);

	/**
	 * Sets mesh size.
	 * @param InScale3D 
	 */
	void SetMeshSize(const FVector& InScale3D) const;

protected:
	// Overriden to: check unit creation.
	virtual void BeginPlay() override;
	
	/**
	 * Event for BP on is selected changed
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnIsPathActorSelectedChanged(bool bInIsPathActorSelected);
	
protected:
	// Path point index.
	UPROPERTY(VisibleAnywhere)
	int32 PathPointIndex = -1;

	// Can the actor be edited by the controller.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bCanBeEdit = true;

	// Is actor selected. Used for visual updates.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsPathActorSelected = false;
};

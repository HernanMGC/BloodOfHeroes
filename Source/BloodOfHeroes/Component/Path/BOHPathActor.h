// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// BOH
#include "BOHPathActor.generated.h"

//// ForwardDeclarations
// BOH
class UStaticMeshComponent;

/**
 * Base class for path actor.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class BLOODOFHEROES_API ABOHPathActor : public AActor
{
	GENERATED_BODY()

public:
	// Static mesh component.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent = nullptr;

public:
	/**
	 *  Constructor. Removes tick.
	 */
	ABOHPathActor();
	
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
	 * Set if actor has been selected by controller.
	 * @return 
	 */
	void SetIsPathActorSelected(bool bInIsPathActorSelected);

protected:
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

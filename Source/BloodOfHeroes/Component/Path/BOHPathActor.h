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
	 * Can actor be edit by controller.
	 * @return 
	 */
	FORCEINLINE bool CanBeEdit() const { return bCanBeEdit; };

	/**
	 * Can actor be edit by controller.
	 * @return 
	 */
	FORCEINLINE void SetCanBeEdit(bool bInCanBeEdit) { bCanBeEdit = bInCanBeEdit; };
	
protected:
	// Path point index.
	UPROPERTY(VisibleAnywhere)
	int32 PathPointIndex = -1;

	// Can the actor be edited by the controller.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bCanBeEdit = true;
	
public:
	/**
	 *  Constructor. Removes tick.
	 */
	ABOHPathActor();
};

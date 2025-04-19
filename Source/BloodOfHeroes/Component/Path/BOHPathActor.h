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
};

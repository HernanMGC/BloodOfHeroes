// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"

// BOH
#include "BOHUnitOrders.generated.h"

/**
 * Order types.
 */
UENUM(BlueprintType)
enum class EUnitOrderType : uint8
{
	None UMETA(DisplayName = "None"),
	MoveAlongPath UMETA(DisplayName = "Move Along Path"),
	Stop UMETA(DisplayName = "Stop"),
	Attack UMETA(DisplayName = "Attack"),
	MAX UMETA(Hidden)
};

/**
 * Order state.
 */
UENUM(BlueprintType)
enum class EUnitOrderState : uint8
{
	None UMETA(DisplayName = "None"),
	Queued UMETA(DisplayName = "Queued"),
	Starting UMETA(DisplayName = "Starting"),
	OnGoing UMETA(DisplayName = "On Going"),
	Finished UMETA(DisplayName = "Finished"),
	MAX UMETA(Hidden)
};

/**
 * Order sorting policy.
 */
UENUM(BlueprintType)
enum class EUnitOrderSortingPolicy : uint8
{
	None UMETA(DisplayName = "None"),
	AddToQueue UMETA(DisplayName = "Add To Queue"),
	AddAfterCurrent UMETA(DisplayName = "Add After Current"),
	InterruptCurrentMissable UMETA(DisplayName = "Try Interrupt Current (Discard if not possible)"),
	InterruptCurrentQueueable UMETA(DisplayName = "Try Interrupt Current (After current if not possible)"),
	MAX UMETA(Hidden)
};

/**
 * Unit order.
 */
USTRUCT(BlueprintType)
struct FBOHUnitOrder
{
	GENERATED_BODY()

public:
	// Unit order type.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUnitOrderType OrderType = EUnitOrderType::None;

	// Unit order state.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EUnitOrderState OrderState = EUnitOrderState::Queued;

	// Unit order state.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUnitOrderSortingPolicy OrderSortingPolicy = EUnitOrderSortingPolicy::AddToQueue;

	// Can order be interrupted.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeInterrupted = true;

	// Target actor.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* TargetActor = nullptr;

public:
	/**
	 * Default constructor. It constructs an invalid FBOHUnitOrder.
	 */
	FBOHUnitOrder();

	/**
	 * Explicit constructor.
	 * @param InOrderType
	 * @param InUnitOrderSortingPolicy
	 * @param bInCanBeInterrupted
	 * @param InTargetActor 
	 */
	FBOHUnitOrder(EUnitOrderType InOrderType, EUnitOrderSortingPolicy InUnitOrderSortingPolicy,
	           bool bInCanBeInterrupted, AActor* InTargetActor);

	bool operator==(const FBOHUnitOrder& Other) const;

	/**
	 * Is unit order valid.
	 * @return 
	 */
	bool IsValid() const;
};

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
	None UMETA(DisplayName="None"),
	NextTurn UMETA(DisplayName="Next turn"),
	MAX UMETA(Hidden)
};

/**
 * Order state.
 */
UENUM(BlueprintType)
enum class EUnitOrderState : uint8
{
	None UMETA(DisplayName="None"),
	Pending UMETA(DisplayName="Pending"),
	OnGoing UMETA(DisplayName="On Going"),
	Finished UMETA(DisplayName="Finished"),
	MAX UMETA(Hidden)
};


/**
 * Unit order.
 */
USTRUCT(BlueprintType)
struct FUnitOrder
{
	GENERATED_BODY()

public:
	// Unit order type
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUnitOrderType OrderType = EUnitOrderType::None;

	// Unit order type
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUnitOrderState OrderState = EUnitOrderState::None;

public:
	/**
	 * Default constructor. It constructs an invalid FUnitOrder.
	 */
	FUnitOrder();

	/**
	 * Explicit constructor.
	 * @param InOrderType 
	 */
	FUnitOrder(EUnitOrderType InOrderType);

	bool operator==(const FUnitOrder& Other) const;
	
	/**
	 * Is unit order valid.
	 * @return 
	 */
	bool IsValid() const; 
};
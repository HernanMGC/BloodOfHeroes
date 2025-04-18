// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "GameFramework/Character.h"

// BOH
#include "BOHCharacter.generated.h"

/**
 * Unit types.
 */
UENUM()
enum class EBOHUnitType : uint8
{
	None = 0, // Undefined unit type
	Runner = 1, // Runner unit
	Enforcer = 2, // Enforcer unit
	MAX UMETA(Hidden) // MAX enum number
};

/**
 * Unit base info.
 */
USTRUCT(BlueprintType)
struct FBOHUnitInfo
{
	GENERATED_BODY()

public:
	// Unit ID.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 UnitID = -1;

	// Team ID.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 TeamID = -1;

	// Unit type.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EBOHUnitType UnitType = EBOHUnitType::None;

public:
	/**
	 * Defaults constructor.
	 */
	FBOHUnitInfo();
	
	/**
	 * Explicit constructor for UnitInfo.
	 * @param InUnitID 
	 * @param InTeamID 
	 * @param InUnitType 
	 */
	FBOHUnitInfo(int32 InUnitID, int32 InTeamID, EBOHUnitType InUnitType);
	
	/**
	 * Equal operator for UnitInfo.
	 * @return 
	 */
	bool operator==(const FBOHUnitInfo&) const;

	/**
	 * Checks if Unit info has valid info.
	 * @return 
	 */
	bool IsValid() const;

	/**
	 * Return human friend Unity info to print.
	 * @return 
	 */
	FString ToString() const;
};

/**
 * Base class for unit characters.
 */
UCLASS()
class BLOODOFHEROES_API ABOHCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	/**
	 * Returns Unit info.
	 * @return 
	 */
	FORCEINLINE const FBOHUnitInfo& GetUnitInfo() const { return UnitInfo; }
	
protected:
	// ToDo: This has to be generated, not a hardcoded option.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBOHUnitInfo UnitInfo;
};

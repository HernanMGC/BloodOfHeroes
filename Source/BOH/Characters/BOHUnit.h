// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ModularCharacter.h"

// BOH
#include "BOHUnit.generated.h"

class UAbilitySystemComponent;
class UBOHAbilitySystemComponent;
//// ForwardDeclaration
// UnrealEngine
class UBehaviorTree;

// BOH
class UBOHPawnExtensionComponent;

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

	// Unit speed
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Speed = 0.0f;

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


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitIsSelectedChanged, bool, bNewIsSelected);

/**
 * Base class for unit characters.
 */
UCLASS(Abstract)
class BOH_API ABOHUnit : public AModularCharacter,
                         public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnUnitIsSelectedChanged OnUnitIsSelectedChanged;

public:
	// Constructor. Removes tick.
	ABOHUnit();

	/**
	 * Returns Unit info.
	 * @return 
	 */
	FORCEINLINE const FBOHUnitInfo& GetUnitInfo() const { return UnitInfo; }

	/**
	 * Returns is selected current state.
	 * @return 
	 */
	FORCEINLINE bool IsUnitSelected() const { return bIsUnitSelected; }

	/**
	 * Sets new is selected state.
	 * @param bNewIsSelected 
	 * @return 
	 */
	FORCEINLINE void SetIsUnitSelected(bool bNewIsSelected);

	/**
	 * Returns unit behavior tree.
	 * @return 
	 */
	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

#pragma region AbilitySystemInterface

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "BOH|Character")
	UBOHAbilitySystemComponent* GetBOHAbilitySystemComponent() const;

#pragma endregion

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "TW|Character")
	void OnAbilitySystemInitialized();
	virtual void OnAbilitySystemInitialized_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "TW|Character")
	void OnAbilitySystemUninitialized();
	virtual void OnAbilitySystemUninitialized_Implementation();

	virtual void InitializeGameplayTags() const;

protected:
	// ToDo: This has to be generated, not a hardcoded option.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBOHUnitInfo UnitInfo;

	// Unit behavior tree.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBehaviorTree> BehaviorTree = nullptr;

	// Is unit selected.
	UPROPERTY(Transient)
	bool bIsUnitSelected = false;

	//! The pawn extension component for this character.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BOH|Character", DisplayName="PawnExtComponent",
		Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBOHPawnExtensionComponent> PawnExtComponent;

	//! The ability system component for this character. Set by the ATWCharacter::CacheAbilitySystemComponent function.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TW|Character", DisplayName="AbilitySystemComponent",
		Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBOHAbilitySystemComponent> AbilitySystemComponent;
};

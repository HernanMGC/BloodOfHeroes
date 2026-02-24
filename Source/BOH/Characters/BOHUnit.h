// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Character.h"

// BOH
#include "BOHUnit.generated.h"

//// ForwardDeclaration
// UnrealEngine
class UGameplayEffect;
class UBehaviorTree;

// BOH
class UBOHAbilitySystemComponent;
class UBOHGameplayAbility;
class UBOHUnitAttributeSet;

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

	// Unit speed.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Speed = 0.0f;

	// Unit evasion radius.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float EvasionRadius = 0.0f;

	// Unit reach radius.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ReachRadius = 0.0f;

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
	 * @param InSpeed
	 * @param InEvasionRadius
	 * @param InReachRadius 
	 */
	FBOHUnitInfo(int32 InUnitID, int32 InTeamID, EBOHUnitType InUnitType, float InSpeed, float InEvasionRadius, float InReachRadius);

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
class BOH_API ABOHUnit : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Unit selection state change delegate.
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
	 * Sets Unit info.
	 * @param InUnitInfo 
	 */
	FORCEINLINE void SetUnitInfo(const FBOHUnitInfo& InUnitInfo) { UnitInfo = InUnitInfo; }

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

	/**
	 * Returns evasion capsule.
	 * @return 
	 */
	FORCEINLINE UCapsuleComponent* GetEvasionCollider() const { return EvasionCollider; }

	/**
	 * Returns reach capsule.
	 * @return 
	 */
	FORCEINLINE UCapsuleComponent* GetReachCollider() const { return ReachCollider; }
	
protected:
	// Overriden to: Creates ASC and Unit Attr. set.
	virtual void BeginPlay() override;

	// Overriden to: Initializes ASC, abilities and effects.
	virtual void PostInitializeComponents() override;

#pragma region IAbilitySystemInterface
	// Overriden to: Return its ability component.
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
#pragma endregion // IAbilitySystemInterface

	UFUNCTION(BlueprintCallable)
	virtual UBOHAbilitySystemComponent* GetBOHAbilitySystemComponent() const;

	/**
	 * Reacts to speed attribute change to update speed on movement component.
	 * @param OnAttributeChangeData 
	 */
	void OnSpeedAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData);

	/**
	 * Reacts to evasion radius attribute change to evasion capsule radius.
	 * @param OnAttributeChangeData 
	 */
	void OnEvasionRadiusAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData);

	/**
	 * Reacts to reach radius attribute change to reach capsule radius.
	 * @param OnAttributeChangeData 
	 */
	void OnReachRadiusAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData);

	/**
	 * Adds initial abilities to ASC.
	 */
	void InitializeAbilities();

	/**
	 * Adds initial effects to ASC.
	 */
	void InitializeEffects();

	/**
	 * On reach begin overlap try to attack other character if possible.
	 * @param OverlappedComponent 
	 * @param OtherActor 
	 * @param OtherComp 
	 * @param OtherBodyIndex 
	 * @param bFromSweep 
	 * @param SweepResult 
	 */
	UFUNCTION()
	void OnReachBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                         UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep,
	                         const FHitResult& SweepResult);

protected:
	// ToDo: This has to be generated, not a hardcoded option.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BOH|Unit")
	FBOHUnitInfo UnitInfo;

	// Unit behavior tree.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBehaviorTree> BehaviorTree = nullptr;

	// Is unit selected.
	UPROPERTY(Transient)
	bool bIsUnitSelected = false;

	// Ability component reference.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BOH|GAS")
	TObjectPtr<UBOHAbilitySystemComponent> ASC = nullptr;

	// Basic Unit attribute set.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BOH|GAS")
	TObjectPtr<UBOHUnitAttributeSet> UnitSet = nullptr;

	// Initial abilities.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|GAS")
	TArray<TSubclassOf<UBOHGameplayAbility>> DefaultAbilities;

	// Unit initialization gameplay effect.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|GAS")
	TSubclassOf<UGameplayEffect> UnitInitializationEffect;

	// Initial gameplay effects.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|GAS")
	TArray<TSubclassOf<UGameplayEffect>> DefaultGameplayEffects;

	// Evasion radius capsule.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BOH|Unit|Collisions")
	TObjectPtr<UCapsuleComponent> EvasionCollider = nullptr;

	// Reach radius capsule.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BOH|Unit|Collisions")
	TObjectPtr<UCapsuleComponent> ReachCollider = nullptr;
};

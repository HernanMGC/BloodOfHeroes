// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"

// BOH
#include "BOHAbilitySet.generated.h"

//// ForwardDeclarations
// UnrealEngine
class UAttributeSet;
class UGameplayEffect;
struct FActiveGameplayEffectHandle;

// BOH
class UBOHAbilitySystemComponent;
class UBOHGameplayAbility;

/**
 *	Data used by the ability set to grant gameplay abilities.
 */
USTRUCT(BlueprintType)
struct BOH_API FBOHAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:
	// Gameplay ability to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBOHGameplayAbility> Ability = nullptr;

	// Level of ability to grant.
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	// Tag used to process input for the ability.
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Input"))
	FGameplayTag InputTag;

	//! When removing this granted ability, if wait until it ends
	UPROPERTY(EditDefaultsOnly)
	bool WaitAbilityEndToRemove = false;
};

/**
 *	Data used by the ability set to grant gameplay effects.
 */
USTRUCT(BlueprintType)
struct BOH_API FBOHAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:
	// Gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	// Level of gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

/**
 *	Data used by the ability set to grant attribute sets.
 */
USTRUCT(BlueprintType)
struct BOH_API FBOHAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	// Gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;

	// Attribute set data table.
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UDataTable> AttributeTable;
};

/**
 *	Data used to store handles to what has been granted by the ability set.
 */
USTRUCT(BlueprintType)
struct BOH_API FBOHAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle, bool WaitAbilityEndToRemove = false);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);

	void TakeFromAbilitySystem(UBOHAbilitySystemComponent* BOHASC);

protected:
	// Handles to the granted abilities.
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	// Handles to the granted abilities that have to wait to end to remove them
	UPROPERTY()
	TSet<FGameplayAbilitySpecHandle> WaitEndToRemoveAbilitySpecHandles;

	// Handles to the granted gameplay effects.
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	// Pointers to the granted attribute sets
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};


/**
 *	Non-mutable data asset used to grant gameplay abilities and gameplay effects.
 */
UCLASS(BlueprintType, Const)
class BOH_API UBOHAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UBOHAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Grants the ability set to the specified ability system component.
	// The returned handles can be used later to take away anything that was granted.
	virtual void GiveToAbilitySystem(UBOHAbilitySystemComponent* BOHASC, FBOHAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;

protected:
	// Gameplay abilities to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta=(TitleProperty=Ability))
	TArray<FBOHAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	// Gameplay effects to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta=(TitleProperty=GameplayEffect))
	TArray<FBOHAbilitySet_GameplayEffect> GrantedGameplayEffects;

	// Attribute sets to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta=(TitleProperty=AttributeSet))
	TArray<FBOHAbilitySet_AttributeSet> GrantedAttributes;
};

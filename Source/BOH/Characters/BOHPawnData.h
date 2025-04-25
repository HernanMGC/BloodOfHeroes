// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

// BOH
#include "BOHPawnData.generated.h"

//// ForwardDeclarations
// BOH
class UBOHAbilitySet;
class UBOHAbilityTagRelationshipMapping;
class UBOHInputConfig;

/**
 * Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "BOH Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class BOH_API UBOHPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UBOHPawnData(const FObjectInitializer& ObjectInitializer);

public:
	// Class to instantiate for this pawn (should usually derive from ABOHPawn or ABOHCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Abilities")
	TArray<TObjectPtr<UBOHAbilitySet>> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Abilities")
	TObjectPtr<UBOHAbilityTagRelationshipMapping> TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Input")
	TObjectPtr<UBOHInputConfig> InputConfig;
};

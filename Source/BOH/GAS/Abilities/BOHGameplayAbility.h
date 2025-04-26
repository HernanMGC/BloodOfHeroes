// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"

// BOH
#include "BOH/BOH.h"
#include "BOHGameplayAbility.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class BOH_API UBOHGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	// Ability input ID.
	UPROPERTY(EditAnywhere, Category = "BOHAbility")
	EAbilityInputID AbilityInputID = EAbilityInputID::None;

public:
	/**
	 * Returns Ability Input ID.
	 * @return 
	 */
	FORCEINLINE EAbilityInputID GetAbilityInputID() const { return  AbilityInputID; };
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// Unreal
#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"

// BOH
#include "BOHAbilitySystemComponent.generated.h"

/**
 * Base class for BOH ability system component.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BOH_API UBOHAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
};

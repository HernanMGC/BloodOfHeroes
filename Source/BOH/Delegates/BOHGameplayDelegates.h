// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// Unreal
#include "CoreMinimal.h"

//// ForwardDeclarations
// Unreal
class APawn;
class AController;
class UAbilitySystemComponent;

/**
 * 
 */
class BOH_API FBOHGameplayDelegates
{
public:
	DECLARE_MULTICAST_DELEGATE_TwoParams(FPawnSpawned, AController*, APawn*);
	static FPawnSpawned OnPlayerSpawned;
	static FPawnSpawned OnPawnSpawned;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FPawnReady, AController*, APawn*);
	static FPawnReady OnPlayerReady;
	static FPawnReady OnPawnReady;

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FPawnAbilitySystemReady, AController*, APawn*, UAbilitySystemComponent*);
	static FPawnAbilitySystemReady OnPlayerAbilitySystemReady;
	static FPawnAbilitySystemReady OnPawnAbilitySystemReady;
};

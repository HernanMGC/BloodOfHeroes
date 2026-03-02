// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "BOH/Utils/BOHUtils.h"

// BOH
#include "BOHGameplayMessage.generated.h"

class ABOHUnit;
/**
 * Empty message.
 */
USTRUCT(BlueprintType)
struct FBOHSimpleEmptyMessage
{
	GENERATED_BODY()
};

/**
 * Single tag message.
 */
USTRUCT(BlueprintType)
struct FBOHSimpleTagMessage
{
	GENERATED_BODY()

	// Simple tag message.
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag Tag = FGameplayTag::EmptyTag;
};

/**
 * Tag container message.
 */
USTRUCT(BlueprintType)
struct FBOHSimpleTagContainerMessage
{
	GENERATED_BODY()

	// Simple tag message.
	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer Tag = FGameplayTagContainer();
};

/**
 * Sender authorized message.
 */
USTRUCT(BlueprintType)
struct FBOHSenderAuthorizedMessage
{
	GENERATED_BODY()

	// Sender pointer for auth validation.
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Sender = nullptr;
};

/**
 * Unit path update message.
 */
USTRUCT(BlueprintType)
struct FBOHUnitPathUpdateMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ABOHUnit> Unit = nullptr;
	
	// Sender pointer for auth validation.
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> Path;
};
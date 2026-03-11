// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// BOH
#include "BOHGameplayMessage.generated.h"

//// ForwardDeclarations
// BOH
class ABOHPlayerController;
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
 * Sender authorized sring message.
 */
USTRUCT(BlueprintType)
struct FBOHAuthorizedTextMessage : public FBOHSenderAuthorizedMessage
{
	GENERATED_BODY()

	// String Message.
	UPROPERTY(BlueprintReadWrite)
	FString String = TEXT("");
};

/**
 * Sender authorized Int32 message.
 */
USTRUCT(BlueprintType)
struct FBOHAuthorizedInt32Message : public FBOHSenderAuthorizedMessage
{
	GENERATED_BODY()

	// Int32 Message.
	UPROPERTY(BlueprintReadWrite)
	int32 Number = INDEX_NONE;
};

/**
 * Sender authorized float message.
 */
USTRUCT(BlueprintType)
struct FBOHAuthorizedFloatMessage : public FBOHSenderAuthorizedMessage
{
	GENERATED_BODY()

	// Float Message.
	UPROPERTY(BlueprintReadWrite)
	float Number = 0.f;
};

/**
 * Selected unit message.
 */
USTRUCT(BlueprintType)
struct FBOHSelectedUnitMessage
{
	GENERATED_BODY()

	// Sender pointer for auth validation.
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ABOHPlayerController> PlayerController = nullptr;

	// Sender pointer for auth validation.
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ABOHUnit> Unit = nullptr;
};

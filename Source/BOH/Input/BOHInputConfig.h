// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "InputTriggers.h"

// BOH
#include "BOHInputConfig.generated.h"

//// ForwardDeclarations
// UnrealEngine
class UInputAction;
class UInputMappingContext;

// BOH
class UBOHLocalPlayer;

/**
 * Struct used to map a input action to a gameplay input tag.
 */
USTRUCT(BlueprintType)
struct BOH_API FBOHInputAction
{
	GENERATED_BODY()

public:	
	// The input action.
	UPROPERTY(EditDefaultsOnly)
	const UInputAction* InputAction = nullptr;

	// The Tag this input action relates to.
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Input"))
	FGameplayTag InputTag;

	// How long the input is buffered for (0 == not buffered)
	UPROPERTY(EditDefaultsOnly, Category="Buffering", meta=(ClampMin="0.0"))
	float BufferTime = 0.0f;

	// Use the input action event Start as if it was Triggered.
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="IsAbilityAction", EditConditionHides))
	ETriggerEvent TriggerEvent = ETriggerEvent::Triggered;

	// Will flag this input as an ability action.
	UPROPERTY()
	bool IsAbilityAction = false;
	
public:
	/**
	 * @brief Can this input be buffered.
	 */
	FORCEINLINE bool CanBuffer() const { return BufferTime > 0.0f; };

	/**
	 * @brief Should this input be removed from the buffer.
	 * @return True when the duration was exceeded, false otherwise.
	 */
	FORCEINLINE bool ShouldBeRemoved(const float CurrentDuration) const { return CurrentDuration > BufferTime; };
};

/**
 * @struct UBOHInputConfig
 * @brief Non-mutable data asset that contains input configuration properties.
 */
UCLASS(BlueprintType, Const)
class BOH_API UBOHInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:	
	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and must be manually bound.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input", Meta = (TitleProperty = "InputTag"))
	TArray<FBOHInputAction> NativeInputActions;

	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and are automatically bound to abilities with matching input tags.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input", Meta = (TitleProperty = "InputTag"))
	TArray<FBOHInputAction> AbilityInputActions;

	// Confirm input action. Used in abilities to confirm targeting.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input", Meta = (TitleProperty = "InputTag"))
	FBOHInputAction ConfirmInputAction;

	// Cancel input action. Used in abilities to cancel targeting.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input", Meta = (TitleProperty = "InputTag"))
	FBOHInputAction CancelInputAction;
	
public:
	UBOHInputConfig(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool LogNotFound = true) const;
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool LogNotFound = true) const;

	const FBOHInputAction* FindNativeBOHInputActionForTag(const FGameplayTag& InputTag, bool LogNotFound = true) const;
	const FBOHInputAction* FindAbilityBOHInputActionForTag(const FGameplayTag& InputTag, bool LogNotFound = true) const;

protected:

#if WITH_EDITOR
	virtual void UpdateInputActions();
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
};

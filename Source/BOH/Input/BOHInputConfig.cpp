// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHInputConfig.h"

// UnrealEngine
#include "InputMappingContext.h"

// BOH
#include "BOH/BOHLogs.h"

//////////////////////////////////////////////////////////////////////////
// UBOHInputConfig
//////////////////////////////////////////////////////////////////////////

UBOHInputConfig::UBOHInputConfig(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

const UInputAction* UBOHInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool LogNotFound) const
{
	for (const FBOHInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (LogNotFound)
	{
		UE_LOG(LogBOH, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

const UInputAction* UBOHInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool LogNotFound) const
{
	for (const FBOHInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (LogNotFound)
	{
		UE_LOG(LogBOH, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

const FBOHInputAction* UBOHInputConfig::FindNativeBOHInputActionForTag(const FGameplayTag& InputTag, bool LogNotFound) const
{
	const FBOHInputAction* result = NativeInputActions.FindByPredicate
	(
		[&InputTag](const FBOHInputAction& Other)
		{
			return Other.InputTag == InputTag;
		}
	);

	if (LogNotFound && result == nullptr)
	{
		UE_LOG(LogBOH, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

const FBOHInputAction* UBOHInputConfig::FindAbilityBOHInputActionForTag(const FGameplayTag& InputTag, bool LogNotFound) const
{
	const FBOHInputAction* result = AbilityInputActions.FindByPredicate
	(
		[&InputTag](const FBOHInputAction& Other)
		{
			return Other.InputTag == InputTag;
		}
	);

	if (LogNotFound && result == nullptr)
	{
		UE_LOG(LogBOH, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return result;
}

#if WITH_EDITOR

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHInputConfig::UpdateInputActions()
{
	for (FBOHInputAction& nativeAction : NativeInputActions)
	{
		nativeAction.IsAbilityAction = false; 
	}
	
	for (FBOHInputAction& abilityAction : AbilityInputActions)
	{
		abilityAction.IsAbilityAction = true; 
	}

	ConfirmInputAction.IsAbilityAction = false;
	CancelInputAction.IsAbilityAction = false;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHInputConfig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	UpdateInputActions();
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHInputConfig::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	UpdateInputActions();
}

#endif
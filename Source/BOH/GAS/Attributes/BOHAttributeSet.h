// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "AttributeSet.h"

// BOH
#include "BOHAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * Base attribute set class.
 */
UCLASS(Abstract)
class BOH_API UBOHAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

protected:
	// Overriden to: Apply clamp.
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	// Overriden to: Apply clamp.
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/**
	 * Function to be implemented on child attribute sets if clamping is needed.
	 * @param Attribute 
	 * @param NewValue 
	 */
	virtual void ClampAttributeOnChange(const FGameplayAttribute& Attribute, float& NewValue) const PURE_VIRTUAL(,);
};

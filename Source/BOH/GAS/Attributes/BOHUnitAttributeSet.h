// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"

// BOH
#include "BOHAttributeSet.h"
#include "BOHUnitAttributeSet.generated.h"

/**
 * Basic attribute set for units. It will handle speed and interaction radius.
 */
UCLASS()
class BOH_API UBOHUnitAttributeSet : public UBOHAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "BOH|Attributes")
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(UBOHUnitAttributeSet, Speed)
	
	UPROPERTY(BlueprintReadOnly, Category = "BOH|Attributes")
	FGameplayAttributeData BlockingRadius;
	ATTRIBUTE_ACCESSORS(UBOHUnitAttributeSet, BlockingRadius)
	
	UPROPERTY(BlueprintReadOnly, Category = "BOH|Attributes")
	FGameplayAttributeData EvasionRadius;
	ATTRIBUTE_ACCESSORS(UBOHUnitAttributeSet, EvasionRadius)
	
public:
	/**
	 * 
	 */
	UBOHUnitAttributeSet();

protected:
	// Overriden to: Apply clamp for 0.f as min value.
	virtual void ClampAttributeOnChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
};

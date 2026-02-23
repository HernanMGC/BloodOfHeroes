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
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Speed, Category = "BOH|Attributes")
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(UBOHUnitAttributeSet, Speed)
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ReachRadius, Category = "BOH|Attributes")
	FGameplayAttributeData ReachRadius;
	ATTRIBUTE_ACCESSORS(UBOHUnitAttributeSet, ReachRadius)
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_EvasionRadius, Category = "BOH|Attributes")
	FGameplayAttributeData EvasionRadius;
	ATTRIBUTE_ACCESSORS(UBOHUnitAttributeSet, EvasionRadius)
	
public:
	/**
	 * 
	 */
	UBOHUnitAttributeSet();

	// Overriden to: Add attributes to replicated variables.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Overriden to: Apply clamp for 0.f as min value.
	virtual void ClampAttributeOnChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	/**
	* Called when new Speed value arrives over the network. 
	*/
	UFUNCTION()
	virtual void OnRep_Speed(const FGameplayAttributeData& OldSpeed);
	/**
	* Called when new ReachRadius value arrives over the network. 
	*/
	UFUNCTION()
	virtual void OnRep_ReachRadius(const FGameplayAttributeData& OldReachRadius);
	/**
	* Called when new EvasionRadius value arrives over the network. 
	*/
	UFUNCTION()
	virtual void OnRep_EvasionRadius(const FGameplayAttributeData& OldEvasionRadius);
};

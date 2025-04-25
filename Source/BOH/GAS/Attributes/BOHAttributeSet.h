// Copyright Tequila Works S.L. 2022

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

#include "BOHAttributeSet.generated.h"


struct FGameplayEffectSpec;
class UBOHAbilitySystemComponent;


/**
 * This macro defines a set of helper functions for accessing and initializing attributes.
 *
 * The following example of the macro:
 *		ATTRIBUTE_ACCESSORS(UBOHHealthSet, Health)
 * will create the following functions:
 *		static FGameplayAttribute GetHealthAttribute();
 *		float GetHealth() const;
 *		void SetHealth(float NewVal);
 *		void InitHealth(float NewVal);
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


// Delegate used to broadcast attribute events.
DECLARE_MULTICAST_DELEGATE_FourParams(FBOHAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec& /*EffectSpec*/, float /*EffectMagnitude*/);


/**
 * UBOHAttributeSet
 *
 *	Base attribute set class for the project.
 */
UCLASS()
class BOH_API UBOHAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UBOHAttributeSet();

	virtual UWorld* GetWorld() const override;

	/**
	 * @brief Getter for the BOHAbilitySystemComponent this AttributeSet is attached to.
	 * @return BOHAbilitySystemComponent instance.
	 */
	UBOHAbilitySystemComponent* GetBOHAbilitySystemComponent() const;
};
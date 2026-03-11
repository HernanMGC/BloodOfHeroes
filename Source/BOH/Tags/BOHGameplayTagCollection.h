// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"

// BOH
#include "BOHGameplayTagCollection.generated.h"

/**
 * List of native Gameplay Tags
 */
UCLASS(NotBlueprintable)
class BOH_API UBOHGameplayTagCollection : public UObject
{
	GENERATED_BODY()
public:
	// Character Status
	FGameplayTag Tag_Character_Status;
	FGameplayTag Tag_Character_Status_Down;
	FGameplayTag Tag_Character_Status_Pinned;

	// SetByCaller attributes
	FGameplayTag Tag_SetByCaller_UnitAttributeSet_Speed;
	FGameplayTag Tag_SetByCaller_UnitAttributeSet_ReachRadius;
	FGameplayTag Tag_SetByCaller_UnitAttributeSet_EvasionRadius;

	// Messages Channels
	FGameplayTag Tag_MessageChannel_UnitMoveCommand;
	FGameplayTag Tag_MessageChannel_UnitPathUpdate;
	FGameplayTag Tag_MessageChannel_UnitSelected;
	FGameplayTag Tag_MessageChannel_PlayersNameChanged;
	FGameplayTag Tag_MessageChannel_PlayersScoreChanged;
	FGameplayTag Tag_MessageChannel_MatchTotalTimeChanged;
	FGameplayTag Tag_MessageChannel_MatchCurrentTimeChanged;

public:
	static const UBOHGameplayTagCollection& Get() { return *StaticClass()->GetDefaultObject<UBOHGameplayTagCollection>(); }
	
	/**
	 * Initialize all children of UBOHGameplayTagCollection.
	 */
	static FSimpleMulticastDelegate* GetOnGameplayTagCollectionCreatedDelegate();

	/**
	 * Add a tag from the collection to the TagManager.
	 */
	static void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

	/**
	 * Initialize all children of UBOHGameplayTagCollection.
	 */
	UBOHGameplayTagCollection(const FObjectInitializer& Initializer = FObjectInitializer::Get());

	/**
	 * Returns initialization state.
	 */
	FORCEINLINE bool IsInitialized() const { return bIsInitialized; }

protected:
	/**
	 * Override this function to add all tags to the TagManager.
	 */
	virtual void AddAllTags();

	/**
	 * Initialize all children of UBOHGameplayTagCollection.
	 */
	virtual void PostCDOContruct() override;

private:
	// Is Tag collection initialized.
	bool bIsInitialized = false;

	// On Gameplay tag collection created delegate.
	inline static FSimpleMulticastDelegate OnGameplayTagCollectionCreatedDelegate = FSimpleMulticastDelegate();
};

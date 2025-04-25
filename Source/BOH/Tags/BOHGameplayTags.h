// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BOHGameplayTagCollection.h"

#include "BOHGameplayTags.generated.h"

/**
 * @class UBOHGameplayTags
 * @brief This class is used to store all the generic native gameplay tags used in the game.
 */
UCLASS(NotBlueprintable)
class BOH_API UBOHGameplayTags : public UBOHGameplayTagCollection
{
	GENERATED_BODY()
	
public:
	static const UBOHGameplayTags& Get() { return *StaticClass()->GetDefaultObject<UBOHGameplayTags>(); }
	
public:
#pragma region Initialization States
	// Initialization states for the GameFrameworkComponentManager, these are registered in order by BOHGameInstance and some actors will skip right to GameplayReady

	/** Actor/component has initially spawned and can be extended */
	FGameplayTag InitState_Spawned;

	/** All required data has been loaded/replicated and is ready for initialization */
	FGameplayTag InitState_DataAvailable;

	/** The available data has been initialized for this actor/component, but it is not ready for full gameplay */
	FGameplayTag InitState_DataInitialized;

	/** The actor/component is fully ready for active gameplay */
	FGameplayTag InitState_GameplayReady;
#pragma endregion

#pragma region Messages
	FGameplayTag Message_Verb;
	FGameplayTag Message_Verb_Death;
	FGameplayTag Message_Verb_Revive;
	
	FGameplayTag Message_Verb_Damage;
	FGameplayTag Message_Verb_Assist;
	FGameplayTag Message_Verb_Elimination;
	FGameplayTag Message_Verb_SelectedAsAttackTarget;
	
	FGameplayTag Message_Notification;
#pragma endregion

#pragma region Movement Modes
	//! Movement is locked. CMC will report 0 max speed and FRotator::ZeroRotator for rotation.
	FGameplayTag Movement_Blocked;
	
	FGameplayTag Movement_Mode_Walking;
	FGameplayTag Movement_Mode_NavWalking;
	FGameplayTag Movement_Mode_Falling;
	FGameplayTag Movement_Mode_Swimming;
	FGameplayTag Movement_Mode_Flying;
	FGameplayTag Movement_Mode_Custom;
	
	TMap<uint8, FGameplayTag> MovementModeTagMap;
	TMap<uint8, FGameplayTag> CustomMovementModeTagMap;
#pragma endregion

public:
	UBOHGameplayTags(const FObjectInitializer& Initializer = FObjectInitializer::Get());
	
	virtual void AddAllTags() override;

	/**
	 * @brief Add a new MovementMode tag. These tags are linked with a MovementMode enum.
	 * @param OutTag The registered tag.
	 * @param TagName The tags name.
	 * @param MovementMode The MovementMode enum value as uint8.
	 */
	void AddMovementModeTag(FGameplayTag& OutTag, const ANSICHAR* TagName, uint8 MovementMode);
	
	/**
	 * @brief Add a new CustomMovementMode tag. These tags are linked with a CustomMovementMode enum.
	 * @param OutTag The registered tag.
	 * @param TagName The tags name.
	 * @param CustomMovementMode The CustomMovementMode enum value as uint8.
	 */
	void AddCustomMovementModeTag(FGameplayTag& OutTag, const ANSICHAR* TagName, uint8 CustomMovementMode);
};
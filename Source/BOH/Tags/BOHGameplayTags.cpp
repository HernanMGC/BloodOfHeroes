// Fill out your copyright notice in the Description page of Project Settings.

#include "BOH/Tags/BOHGameplayTags.h"

#include "Engine/EngineTypes.h"
#include "GameplayTagsManager.h"

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////

UBOHGameplayTags::UBOHGameplayTags(const FObjectInitializer& Initializer) : Super(Initializer)
{
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////

void UBOHGameplayTags::AddAllTags()
{
	// @TODO: [Jeff.Onsea] Move all tags to their contextual UBOHGameplayTagCollection like we did with BOHInputTags.
	
	AddTag(InitState_Spawned, "InitState.Spawned", "1: Actor/component has initially spawned and can be extended");
	AddTag(InitState_DataAvailable, "InitState.DataAvailable", "2: All required data has been loaded/replicated and is ready for initialization");
	AddTag(InitState_DataInitialized, "InitState.DataInitialized", "3: The available data has been initialized for this actor/component, but it is not ready for full gameplay");
	AddTag(InitState_GameplayReady, "InitState.GameplayReady", "4: The actor/component is fully ready for active gameplay");

	AddTag(Message_Verb, "Messages.Verb", "Represents a generic message of the form Instigator Verb Target (in Context, with Magnitude).");

	AddTag(Message_Verb_Death, "Messages.Dying", "Message that fires on started dying. This event only fires on the server.");
	AddTag(Message_Verb_Death, "Messages.Death", "Message that fires on death. This event only fires on the server.");
	AddTag(Message_Verb_Revive, "Messages.Revive", "Message that fires on revive. This event only fires on the server.");
	AddTag(Message_Verb_Damage, "Messages.Verb.Damage", "Message that fires on damaged. This event only fires on the server.");
	AddTag(Message_Verb_Assist, "Messages.Verb.Assist", "Message that fires on assist (someone we helped kill has died). This event only fires on the server.");
	AddTag(Message_Verb_Elimination, "Messages.Verb.Elimination", "Message that fires on elimination (Killed someone). This event only fires on the server.");
	AddTag(Message_Verb_SelectedAsAttackTarget, "Messages.Verb.SelectedAsAttackTarget", "Message that fires on being selected as target. This event only fires on the server.");

	AddTag(Message_Notification, "Messages.Notification", "A message destined for a transient log (e.g., an elimination feed or inventory pickup stream)");
	
	AddTag(Movement_Blocked, "Movement.Blocked", "Movement is stopped. CMC will report zero max speed and delta rotation.");
	AddMovementModeTag(Movement_Mode_Walking, "Movement.Mode.Walking", MOVE_Walking);
	AddMovementModeTag(Movement_Mode_NavWalking, "Movement.Mode.NavWalking", MOVE_NavWalking);
	AddMovementModeTag(Movement_Mode_Falling, "Movement.Mode.Falling", MOVE_Falling);
	AddMovementModeTag(Movement_Mode_Swimming, "Movement.Mode.Swimming", MOVE_Swimming);
	AddMovementModeTag(Movement_Mode_Flying, "Movement.Mode.Flying", MOVE_Flying);
	AddMovementModeTag(Movement_Mode_Custom, "Movement.Mode.Custom", MOVE_Custom);
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////

void UBOHGameplayTags::AddMovementModeTag(FGameplayTag& OutTag, const ANSICHAR* TagName, uint8 MovementMode)
{
	AddTag(OutTag, TagName, "Character movement mode tag.");
	MovementModeTagMap.Add(MovementMode, OutTag);
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////

void UBOHGameplayTags::AddCustomMovementModeTag(FGameplayTag& OutTag, const ANSICHAR* TagName, uint8 CustomMovementMode)
{
	AddTag(OutTag, TagName, "Character custom movement mode tag.");
	CustomMovementModeTagMap.Add(CustomMovementMode, OutTag);
}

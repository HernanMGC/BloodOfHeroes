// Fill out your copyright notice in the Description page of Project Settings.
#include "BOHGameplayTagCollection.h"
#include "GameplayTagsManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogBOHGameplayTagCollection, All, All);

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////

UBOHGameplayTagCollection::UBOHGameplayTagCollection(const FObjectInitializer& Initializer) : Super(Initializer)
{
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////

FSimpleMulticastDelegate* UBOHGameplayTagCollection::GetOnGameplayTagCollectionCreatedDelegate()
{
	return &OnGameplayTagCollectionCreatedDelegate;
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////

void UBOHGameplayTagCollection::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	UGameplayTagsManager& tagsManager = UGameplayTagsManager::Get();
	OutTag = tagsManager.AddNativeGameplayTag
	(
		FName(TagName),
		FString(TEXT("(Native) ")) + FString(TagComment)
	);
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////

void UBOHGameplayTagCollection::AddAllTags()
{
	// Character Status
	AddTag(Tag_Character_Status, "Character.Status", "Character status.");
	AddTag(Tag_Character_Status_Down, "Character.Status.Down", "Character status when attacked and put down.");
	AddTag(Tag_Character_Status_Pinned, "Character.Status.Pinned", "Character status when attacked and pinned by other character.");

	// SetByCaller attributes
	AddTag(Tag_SetByCaller_UnitAttributeSet_Speed, "SetByCaller.UnitAttributeSet.Speed", "Set by caller tags for Speed on Unit Attribute Set.");
	AddTag(Tag_SetByCaller_UnitAttributeSet_ReachRadius, "SetByCaller.UnitAttributeSet.ReachRadius", "Set by caller tags for ReachRadius on Unit Attribute Set.");
	AddTag(Tag_SetByCaller_UnitAttributeSet_EvasionRadius, "SetByCaller.UnitAttributeSet.EvasionRadius", "Set by caller tags for EvasionRadius on Unit Attribute Set.");

	// Messages Channels
	AddTag(Tag_MessageChannel_UnitMoveCommand, "MessageChannel.UnitMoveCommand", "Message channel for Unit Move Commands.");
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////

void UBOHGameplayTagCollection::PostCDOContruct()
{
	UObject::PostCDOContruct();

	AddAllTags();
	bIsInitialized = true;
	
	OnGameplayTagCollectionCreatedDelegate.Broadcast();
}

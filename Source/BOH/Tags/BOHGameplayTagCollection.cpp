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

// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHPawnExtensionComponent.h"

#include "BOH/BOHLogs.h"
#include "BOH/Characters/BOHPawnData.h"
#include "BOH/GAS/BOHAbilitySystemComponent.h"

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

UBOHPawnExtensionComponent::UBOHPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHPawnExtensionComponent::SetPawnData(const UBOHPawnData* InPawnData)
{
	check(InPawnData);

	APawn* pawn = GetPawnChecked<APawn>();

	if (pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData == InPawnData)
	{
		return;
	}

	PawnData = InPawnData;

	pawn->ForceNetUpdate();
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHPawnExtensionComponent::InitializeAbilitySystem(UBOHAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	check(InASC);
	check(InOwnerActor);

	if (AbilitySystemComponent == InASC)
	{
		// The ability system component hasn't changed.
		return;
	}

	if (AbilitySystemComponent)
	{
		// Clean up the old ability system component.
		UninitializeAbilitySystem();
	}

	APawn* pawn = GetPawnChecked<APawn>();
	const AActor* existingAvatar = InASC->GetAvatarActor();

	UE_LOG(LogBOH, Verbose, TEXT("Setting up ASC [%s] on pawn [%s] owner [%s], existing [%s] "), *GetNameSafe(InASC), *GetNameSafe(pawn), *GetNameSafe(InOwnerActor), *GetNameSafe(existingAvatar));

	if ((existingAvatar != nullptr) && (existingAvatar != pawn))
	{
		UE_LOG(LogBOH, Verbose, TEXT("Existing avatar (authority=%s)"), existingAvatar->HasAuthority() ? TEXT("True") : TEXT("False"));

		if (UBOHPawnExtensionComponent* OtherExtensionComponent = FindPawnExtensionComponent(existingAvatar))
		{
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}

	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, pawn);

	if (ensure(PawnData))
	{
		InASC->SetTagRelationshipMapping(PawnData->TagRelationshipMapping);
	}

	OnAbilitySystemInitialized.Broadcast();
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHPawnExtensionComponent::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		FGameplayTagContainer AbilityTypesToIgnore;

		AbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);
		AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
			AbilitySystemComponent->ClearActorInfo();
		}

		OnAbilitySystemUninitialized.Broadcast();
	}

	AbilitySystemComponent = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

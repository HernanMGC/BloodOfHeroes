// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHAbilitySet.h"

// UnrealEngine
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"

// BOH
#include "BOHAbilitySystemComponent.h"
#include "BOHGameplayAbility.h"
#include "BOH/BOHLogs.h"

//////////////////////////////////////////////////////////////////////////
// FBOHAbilitySet_GrantedHandles
//////////////////////////////////////////////////////////////////////////

void FBOHAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle, bool WaitAbilityEndToRemove)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);

		if (WaitAbilityEndToRemove)
		{
			WaitEndToRemoveAbilitySpecHandles.Add(Handle);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void FBOHAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void FBOHAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void FBOHAbilitySet_GrantedHandles::TakeFromAbilitySystem(UBOHAbilitySystemComponent* TWASC)
{
	check(TWASC);

	if (!TWASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			if (WaitEndToRemoveAbilitySpecHandles.Contains(Handle))
			{
				TWASC->SetRemoveAbilityOnEnd(Handle);
			}
			else
			{
				TWASC->ClearAbility(Handle);
			}
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			TWASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		TWASC->RemoveSpawnedAttribute(Set);
	}

	AbilitySpecHandles.Reset();
	WaitEndToRemoveAbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

//////////////////////////////////////////////////////////////////////////
// UBOHAbilitySet
//////////////////////////////////////////////////////////////////////////

UBOHAbilitySet::UBOHAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySet::GiveToAbilitySystem(UBOHAbilitySystemComponent* TWASC, FBOHAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(TWASC);

	if (!TWASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	// Grant the attribute sets.
	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	{
		const FBOHAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		if (!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(LogBOHAbilitySystem, Error, TEXT("GrantedAttributes[%d] on ability set [%s] is not valid"), SetIndex, *GetNameSafe(this));
			continue;
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(TWASC->GetOwner(), SetToGrant.AttributeSet);
		TWASC->AddAttributeSetSubobject(NewSet);

		if (IsValid(SetToGrant.AttributeTable))
		{
			NewSet->InitFromMetaDataTable(SetToGrant.AttributeTable);
		}

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}

	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FBOHAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogBOHAbilitySystem, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		UBOHGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UBOHGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = TWASC->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle, AbilityToGrant.WaitAbilityEndToRemove);
		}
	}

	// Grant the gameplay effects.
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FBOHAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogBOHAbilitySystem, Error, TEXT("GrantedGameplayEffects[%d] on ability set [%s] is not valid"), EffectIndex, *GetNameSafe(this));
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = TWASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, TWASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

// Copyright Tequila Works S.L. 2022

#include "BOH/GAS/BOHAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "BOH/BOHLogs.h"

#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"

#include "BOH/GAS/BOHGameplayAbility.h"
#include "BOH/GAS/BOHAbilityTagRelationshipMapping.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "BOH/Tags/BOHAbilityTags.h"
#include "BOH/Tags/BOHInputTags.h"

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

UBOHAbilitySystemComponent::UBOHAbilitySystemComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();

	FMemory::Memset(ActivationGroupCounts, 0, sizeof(ActivationGroupCounts));
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(Spec);

	if (UBOHGameplayAbility* ability = Cast<UBOHGameplayAbility>(Spec.Ability))
	{
		for (const FAbilityTriggerData& triggerData : ability->AbilityCancelTriggers)
		{
			FGameplayTag eventTag = triggerData.TriggerTag;

			auto& cancelledAbilityMap = (triggerData.TriggerSource == EGameplayAbilityTriggerSource::GameplayEvent) ? GameplayEventCancelledAbilities : OwnedTagCancelledAbilities;

			if (cancelledAbilityMap.Contains(eventTag))
			{
				// Fixme: is this right? Do we want to trigger the ability directly of the spec?
				cancelledAbilityMap[eventTag].AddUnique(Spec.Handle);
			}
			else
			{
				TArray<FGameplayAbilitySpecHandle> triggers;
				triggers.Add(Spec.Handle);
				cancelledAbilityMap.Add(eventTag, triggers);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UBOHGlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<UBOHGlobalAbilitySystem>(GetWorld()))
	{
		GlobalAbilitySystem->UnregisterASC(this);
	}

	Super::EndPlay(EndPlayReason);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

int32 UBOHAbilitySystemComponent::HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	int32 cancelledCount = 0;
	FGameplayTag currentTag = EventTag;

	ABILITYLIST_SCOPE_LOCK();
	
	while (currentTag.IsValid())
	{
		if (GameplayEventCancelledAbilities.Contains(currentTag))
		{
			TArray<FGameplayAbilitySpecHandle> cancelledAbilityHandles = GameplayEventCancelledAbilities[currentTag];

			for (const FGameplayAbilitySpecHandle& abilityHandle : cancelledAbilityHandles)
			{
				if (CancelAbilityFromGameplayEvent(abilityHandle, AbilityActorInfo.Get(), EventTag, Payload, *this))
				{
					cancelledCount++;
				}
			}
		}

		currentTag = currentTag.RequestDirectParent();
	}

	return Super::HandleGameplayEvent(EventTag, Payload);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

bool UBOHAbilitySystemComponent::CancelAbilityFromGameplayEvent(FGameplayAbilitySpecHandle Handle, FGameplayAbilityActorInfo* ActorInfo, FGameplayTag EventTag, const FGameplayEventData* Payload, UAbilitySystemComponent& Component)
{
	FGameplayAbilitySpec* spec = FindAbilitySpecFromHandle(Handle);
	if (!ensureMsgf(spec, TEXT("Failed to find gameplay ability spec %s"), *EventTag.ToString()))
	{
		return false;
	}

	const UGameplayAbility* instancedAbility = spec->GetPrimaryInstance();
	const UGameplayAbility* ability = instancedAbility ? instancedAbility : spec->Ability;
	if (!ensure(ability))
	{
		return false;
	}

	if (!ensure(Payload))
	{
		return false;
	}

	// Make a temp copy of the payload, and copy the event tag into it
	FGameplayEventData tempEventData = *Payload;
	tempEventData.EventTag = EventTag;

	// Run on the non-instanced ability
	CancelAbilitySpec(*spec, nullptr);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);

	const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor);

	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (bHasNewPawnAvatar)
	{
		// Notify all abilities that a new pawn avatar has been set
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			UBOHGameplayAbility* BOHAbilityCDO = CastChecked<UBOHGameplayAbility>(AbilitySpec.Ability);

			if (BOHAbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
			{
				TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
				for (UGameplayAbility* AbilityInstance : Instances)
				{
					UBOHGameplayAbility* BOHAbilityInstance = CastChecked<UBOHGameplayAbility>(AbilityInstance);
					BOHAbilityInstance->OnPawnAvatarSet();
				}
			}
			else
			{
				BOHAbilityCDO->OnPawnAvatarSet();
			}
		}

		// Register with the global system once we actually have a pawn avatar. We wait until this time since some globally-applied effects may require an avatar.
		if (UBOHGlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<UBOHGlobalAbilitySystem>(GetWorld()))
		{
			GlobalAbilitySystem->RegisterASC(this);
		}

		if (UBOHAnimInstance* BOHAnimInst = Cast<UBOHAnimInstance>(ActorInfo->GetAnimInstance()))
		{
			BOHAnimInst->InitializeWithAbilitySystem(this);
		}

		TryActivateAbilitiesOnSpawn();
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::ProcessAbilityActivationBuffer(float DeltaTime, bool bGamePaused)
{	
	// Process buffered activations.
	for (int32 i = BufferedActivationsSpecHandles.Num() - 1; i >= 0; i--)
	{
		const FBOHAbilityActivationBufferData& bufferData = BufferedActivationsSpecHandles[i];
		const float currentTime = GetWorld()->GetTimeSeconds();
		
		if (bufferData.CDO->CanActivateAbility(bufferData.AbilitySpecHandle, AbilityActorInfo.Get(), nullptr, nullptr, nullptr))
		{
			BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Consuming buffered activation for %s."), *bufferData.AbilitySpecHandle.ToString());

			if (!TryActivateAbility(bufferData.AbilitySpecHandle))
			{
				BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Buffered activation for %s failed."), *bufferData.AbilitySpecHandle.ToString())
			}

			ClearBufferBySpecHandle(bufferData.AbilitySpecHandle);
		}
		else if (!bufferData.CDO->DoesAbilitySatisfyBufferTagRequirements(*this, nullptr) || bufferData.ShouldBeRemoved(currentTime))
		{
			BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Removing buffered activation for %s."), *bufferData.AbilitySpecHandle.ToString());
			ClearBufferBySpecHandle(bufferData.AbilitySpecHandle);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

bool UBOHAbilitySystemComponent::RegisterAbilityActivationBuffer(const TSubclassOf<UBOHGameplayAbility> Ability)
{	
	const FGameplayAbilitySpec* spec = FindAbilitySpecFromClass(Ability);
	if (spec == nullptr)
	{
		BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Spec not found for %s while registering activation buffer."), *Ability->GetName());
		return false;
	}

	const UBOHGameplayAbility* cdo = Ability->GetDefaultObject<UBOHGameplayAbility>();
	if (cdo == nullptr)
	{
		BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("CDO not found for %s while registering activation buffer."), *Ability->GetName());
		return false;
	}

	if (!cdo->ShouldBeBuffered())
	{
		BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Ability returned False on ShouldBeBuffered for %s."), *Ability->GetName());
		return false;
	}

	FGameplayTagContainer relevantTags;
	if (!cdo->DoesAbilitySatisfyBufferTagRequirements(*this, &relevantTags))
	{
		BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Ability can't be buffered. Failed tag checks for %s by tags %s."), *Ability->GetName(), *relevantTags.ToString());
		return false;
	}

	const float currentTime = GetWorld()->GetTimeSeconds();
	const FBOHAbilityActivationBufferData data{spec->Handle, currentTime, cdo->BufferDuration, cdo};

	const int32 indexOf = BufferedActivationsSpecHandles.IndexOfByKey(data);
	const bool wasRegistered = indexOf != INDEX_NONE;
	
	if (!wasRegistered)
	{
		BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Registering ability activation buffer for %s."), *BOHGetNameSafe(Ability, true));
		BufferedActivationsSpecHandles.AddUnique(data);

		BlockBuffersWithTags(cdo->BlockBuffersWithTags);
		CancelBuffersWithTags(cdo->CancelBuffersWithTags);
	}
	else
	{
		BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Restarting ability activation buffer for %s. Was already registered."), *BOHGetNameSafe(Ability, true));
		BufferedActivationsSpecHandles[indexOf].BufferStartTime = currentTime;
	}
	
	return wasRegistered;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

bool UBOHAbilitySystemComponent::UnregisterAbilityActivationBuffer(const TSubclassOf<UBOHGameplayAbility> Ability)
{
	const FGameplayAbilitySpec* spec = FindAbilitySpecFromClass(Ability);
	if (spec == nullptr)
	{
		BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Error, TEXT("Spec not found for %s while unregistering activation buffer."), *Ability->GetName());
		return false;
	}

	return ClearBufferBySpecHandle(spec->Handle);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

bool UBOHAbilitySystemComponent::ClearBufferBySpecHandle(const FGameplayAbilitySpecHandle Handle)
{
	const int32 index = BufferedActivationsSpecHandles.IndexOfByPredicate
	(
		[&Handle](const FBOHAbilityActivationBufferData& BufferData)
		{
			return BufferData.AbilitySpecHandle == Handle;
		}
	);

	const FBOHAbilityActivationBufferData& bufferData = BufferedActivationsSpecHandles[index];
	UnBlockBuffersWithTags(bufferData.CDO->BlockBuffersWithTags);

	if (index != INDEX_NONE)
	{
		BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Clearing ability activation buffer for Handle. %s"), *Handle.ToString());
		BufferedActivationsSpecHandles.RemoveAt(index);
	}

	return index != INDEX_NONE;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

bool UBOHAbilitySystemComponent::ClearBufferByAbilityTags(FGameplayTagContainer Tags)
{
	int32 count = 0;
	for (int32 i = BufferedActivationsSpecHandles.Num() - 1; i >= 0; --i)
	{
		// TODO @jeff.onsea add removal of blocking tags.
		const FBOHAbilityActivationBufferData& data = BufferedActivationsSpecHandles[i];
		if (data.CDO != nullptr)
		{
			if (data.CDO->AbilityTags.HasAnyExact(Tags))
			{
				count++;
				BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Clearing ability activation buffer %s for Tags. %s"), *BOHGetNameSafe(data.CDO, true), *Tags.ToString());

				UnBlockBuffersWithTags(data.CDO->BlockBuffersWithTags);

				BufferedActivationsSpecHandles.RemoveAt(i);
			}
		}
	}

	return count > 0;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

int32 UBOHAbilitySystemComponent::ClearBuffer()
{
	BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Clearing ability activation buffer."));
	
	const int32 numRemoved = BufferedActivationsSpecHandles.Num();
	for (int32 i = numRemoved - 1; i >= 0; --i)
	{
		const FBOHAbilityActivationBufferData& data = BufferedActivationsSpecHandles[i];
		UnBlockBuffersWithTags(data.CDO->BlockBuffersWithTags);

		BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Clearing ability activation buffer %s."), *BOHGetNameSafe(data.CDO, true));

		BufferedActivationsSpecHandles.RemoveAt(i);
	}

	return numRemoved;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

bool UBOHAbilitySystemComponent::IsBufferBlockedForAbility(const FGameplayTagContainer& Tags) const
{
	return Tags.HasAnyExact(BlockedBufferTags.GetExplicitGameplayTags());
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::BlockBuffersWithTags(const FGameplayTagContainer& Tags)
{
	BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Blocking buffers with tags %s."), *Tags.ToString());
	
	BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Blocking buffer tag counts:"), *Tags.ToString());
	for (const FGameplayTag& tag : BlockedBufferTags.GetExplicitGameplayTags())
	{
		BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("- %s (%d)."), *tag.ToString(), BlockedBufferTags.GetTagCount(tag));
	}
	
	BlockedBufferTags.UpdateTagCount(Tags, 1);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::UnBlockBuffersWithTags(const FGameplayTagContainer& Tags)
{
	BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Blocking buffers with tags %s."), *Tags.ToString());
	
	BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Blocking buffer tag counts:"), *Tags.ToString());
	for (const FGameplayTag& tag : BlockedBufferTags.GetExplicitGameplayTags())
	{
		BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("- %s (%d)."), *tag.ToString(), BlockedBufferTags.GetTagCount(tag));
	}
	
	BlockedBufferTags.UpdateTagCount(Tags, -1);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::CancelBuffersWithTags(const FGameplayTagContainer& Tags)
{
	ClearBufferByAbilityTags(Tags);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		const UBOHGameplayAbility* BOHAbilityCDO = CastChecked<UBOHGameplayAbility>(AbilitySpec.Ability);
		BOHAbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility)
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (!AbilitySpec.IsActive())
		{
			continue;
		}

		UBOHGameplayAbility* BOHAbilityCDO = CastChecked<UBOHGameplayAbility>(AbilitySpec.Ability);

		if (BOHAbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
		{
			// Cancel all the spawned instances, not the CDO.
			TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
			for (UGameplayAbility* AbilityInstance : Instances)
			{
				UBOHGameplayAbility* BOHAbilityInstance = CastChecked<UBOHGameplayAbility>(AbilityInstance);

				if (ShouldCancelFunc(BOHAbilityInstance, AbilitySpec.Handle))
				{
					if (BOHAbilityInstance->CanBeCanceled())
					{
						BOHAbilityInstance->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), BOHAbilityInstance->GetCurrentActivationInfo(), bReplicateCancelAbility);
					}
					else
					{
						UE_LOG(LogBOHCoreAbilitySystem, Error, TEXT("CancelAbilitiesByFunc: Can't cancel ability [%s] because CanBeCanceled is false."), *BOHAbilityInstance->GetName());
					}
				}
			}
		}
		else
		{
			// Cancel the non-instanced ability CDO.
			if (ShouldCancelFunc(BOHAbilityCDO, AbilitySpec.Handle))
			{
				// Non-instanced abilities can always be canceled.
				check(BOHAbilityCDO->CanBeCanceled());
				BOHAbilityCDO->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), FGameplayAbilityActivationInfo(), bReplicateCancelAbility);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::CancelInputActivatedAbilities(bool bReplicateCancelAbility)
{
	CancelAbilitiesByFunc
	(
		[this](const UBOHGameplayAbility* BOHAbility, FGameplayAbilitySpecHandle Handle)
		{
			const EBOHAbilityActivationPolicy ActivationPolicy = BOHAbility->GetActivationPolicy();
			return ((ActivationPolicy == EBOHAbilityActivationPolicy::OnInputTriggered) || (ActivationPolicy == EBOHAbilityActivationPolicy::WhileInputActive));
		},
		bReplicateCancelAbility
	);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputPress ability task works.
	if (Spec.IsActive())
	{
		// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputRelease ability task works.
	if (Spec.IsActive())
	{
		// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	ProcessAbilityActivationBuffer(DeltaTime, bGamePaused);

	const UBOHAbilityTags& abilityTags = UBOHAbilityTags::Get();
	if (HasMatchingGameplayTag(abilityTags.InputBlocked))
	{
		ClearAbilityInput();
		return;
	}

	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	// @TODO: See if we can use FScopedServerAbilityRPCBatcher ScopedRPCBatcher in some of these loops

	//
	// Process all abilities that activate when the input is held.
	//
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UBOHGameplayAbility* BOHAbilityCDO = CastChecked<UBOHGameplayAbility>(AbilitySpec->Ability);

				if (BOHAbilityCDO->GetActivationPolicy() == EBOHAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	//
	// Process all abilities that had their input pressed this frame.
	//
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				const UBOHGameplayAbility* instance = Cast<UBOHGameplayAbility>(AbilitySpec->GetPrimaryInstance());
				const bool feedInput = (instance != nullptr) ? instance->FeedInputToInstances : true;
				
				if (feedInput && AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputPressed(*AbilitySpec);
					
					if (FGameplayTag inputTag; GetInputTagFromAbilitySpec(AbilitySpec, inputTag))
					{
						BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Consumed input pressed for %s on already active ability."), *inputTag.ToString());
						OnAbilityInputPressedConsumed.Broadcast(inputTag);
					}
				}
				else
				{
					const UBOHGameplayAbility* cdo = CastChecked<UBOHGameplayAbility>(AbilitySpec->Ability);
					if (cdo->GetActivationPolicy() == EBOHAbilityActivationPolicy::OnInputTriggered)
					{
						BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Adding ability to abilities to activate %s."), *BOHGetNameSafe(AbilitySpec->Ability, true));
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	//
	// Try to activate all the abilities that are from presses and holds.
	// We do it all at once so that held inputs don't activate the ability
	// and then also send a input event to the ability because of the press.
	//
	for (const FGameplayAbilitySpecHandle& abilitySpecHandle : AbilitiesToActivate)
	{
		const FGameplayAbilitySpec* abilitySpec = FindAbilitySpecFromHandle(abilitySpecHandle);
		if (abilitySpec == nullptr)
		{
			continue;
		}
		
		const UBOHGameplayAbility* cdo = CastChecked<UBOHGameplayAbility>(abilitySpec->Ability);
		if (cdo == nullptr)
		{
			continue;
		}
		
		if (TryActivateAbility(abilitySpecHandle))
		{
			if (FGameplayTag inputTag; GetInputTagFromAbilitySpec(abilitySpec, inputTag))
			{
				if (cdo->GetActivationPolicy() == EBOHAbilityActivationPolicy::OnInputTriggered)
				{
					BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Consuming input pressed for %s on ability activation %s."), *inputTag.ToString(), *BOHGetNameSafe(cdo, true));
					OnAbilityInputPressedConsumed.Broadcast(inputTag);
				}
				else if (cdo->GetActivationPolicy() == EBOHAbilityActivationPolicy::WhileInputActive)
				{
					BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Consuming input held for %s on ability activation %s."), *inputTag.ToString(), *BOHGetNameSafe(cdo, true));
					OnAbilityInputHeldConsumed.Broadcast(inputTag);
				}
			}
		}
		else
		{
			if (FGameplayTag inputTag; GetInputTagFromAbilitySpec(abilitySpec, inputTag))
			{
				if (cdo->GetActivationPolicy() == EBOHAbilityActivationPolicy::OnInputTriggered)
				{
					BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Not consuming input pressed for %s. Ability activation %s failed."), *inputTag.ToString(), *BOHGetNameSafe(cdo, true));
					if (cdo->ShouldBeBuffered())
					{
						RegisterAbilityActivationBuffer(cdo->GetClass());
					}
				}
				else if (cdo->GetActivationPolicy() == EBOHAbilityActivationPolicy::WhileInputActive)
				{
					BOH_LOG_CLASS(LogBOHCoreAbilitySystem, Verbose, TEXT("Not consuming input held for %s. Ability activation %s failed."), *inputTag.ToString(), *BOHGetNameSafe(cdo, true));
				}
			}
		}
	}

	//
	// Process all abilities that had their input released this frame.
	//
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				const UBOHGameplayAbility* instance = Cast<UBOHGameplayAbility>(AbilitySpec->GetPrimaryInstance());
				const bool feedInput = (instance != nullptr) ? instance->FeedInputToInstances : true;
				if (feedInput && AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputReleased(*AbilitySpec);
					
					if (FGameplayTag inputTag; GetInputTagFromAbilitySpec(AbilitySpec, inputTag))
					{
						OnAbilityInputReleasedConsumed.Broadcast(inputTag);
					}
				}
			}
		}
	}

	//
	// Clear the cached ability handles.
	//
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability)
{
	Super::NotifyAbilityActivated(Handle, Ability);

	UBOHGameplayAbility* BOHAbility = CastChecked<UBOHGameplayAbility>(Ability);

	AddAbilityToActivationGroup(BOHAbility->GetActivationGroup(), BOHAbility);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	Super::NotifyAbilityFailed(Handle, Ability, FailureReason);

#if! UE_BUILD_SHIPPING
	UBOHGameplayAbilityLog::Get().LogAbilityActivateFailed(*Ability, *GetAvatarActor(), FailureReason);
#endif
	
	if (APawn* Avatar = Cast<APawn>(GetAvatarActor()))
	{
		if (!Avatar->IsLocallyControlled() && Ability->IsSupportedForNetworking())
		{
			ClientNotifyAbilityFailed(Ability, FailureReason);
			return;
		}
	}

	HandleAbilityFailed(Ability, FailureReason);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);

	UBOHGameplayAbility* BOHAbility = CastChecked<UBOHGameplayAbility>(Ability);

	RemoveAbilityFromActivationGroup(BOHAbility->GetActivationGroup(), BOHAbility);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags)
{
	FGameplayTagContainer ModifiedBlockTags = BlockTags;
	FGameplayTagContainer ModifiedCancelTags = CancelTags;

	if (TagRelationshipMapping)
	{
		// Use the mapping to expand the ability tags into block and cancel tag
		TagRelationshipMapping->GetAbilityTagsToBlockAndCancel(AbilityTags, &ModifiedBlockTags, &ModifiedCancelTags);
	}

	Super::ApplyAbilityBlockAndCancelTags(AbilityTags, RequestingAbility, bEnableBlockTags, ModifiedBlockTags, bExecuteCancelTags, ModifiedCancelTags);

	//@TODO: Apply any special logic like blocking input or movement
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bCanBeCanceled)
{
	Super::HandleChangeAbilityCanBeCanceled(AbilityTags, RequestingAbility, bCanBeCanceled);

	//@TODO: Apply any special logic like blocking input or movement
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const
{
	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetRequiredAndBlockedActivationTags(AbilityTags, &OutActivationRequired, &OutActivationBlocked);
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

UBOHAttributeSet* UBOHAbilitySystemComponent::GetBOHAttributeSet(TSubclassOf<UBOHAttributeSet> AttributeSetClass) const
{
	// return the pointer
	// const_cast is used to remove the constness of the return value (WE should avoid this, but the API is not const correct)
	return const_cast<UBOHAttributeSet*>(Cast<UBOHAttributeSet>(GetAttributeSubobject(AttributeSetClass)));
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::SetTagRelationshipMapping(UBOHAbilityTagRelationshipMapping* NewMapping)
{
	TagRelationshipMapping = NewMapping;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::ClientNotifyAbilityFailed_Implementation(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	HandleAbilityFailed(Ability, FailureReason);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	if (const UBOHGameplayAbility* BOHAbility = Cast<const UBOHGameplayAbility>(Ability))
	{
		BOHAbility->OnAbilityFailedToActivate(FailureReason);
	}	
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

bool UBOHAbilitySystemComponent::IsActivationGroupBlocked(EBOHAbilityActivationGroup Group) const
{
	bool bBlocked = false;

	switch (Group)
	{
	case EBOHAbilityActivationGroup::Independent:
		// Independent abilities are never blocked.
		bBlocked = false;
		break;

	case EBOHAbilityActivationGroup::Exclusive_Replaceable:
	case EBOHAbilityActivationGroup::Exclusive_Blocking:
		// Exclusive abilities can activate if nothing is blocking.
		bBlocked = (ActivationGroupCounts[(uint8)EBOHAbilityActivationGroup::Exclusive_Blocking] > 0);
		break;

	default:
		checkf(false, TEXT("IsActivationGroupBlocked: Invalid ActivationGroup [%d]\n"), (uint8)Group);
		break;
	}

	return bBlocked;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::AddAbilityToActivationGroup(EBOHAbilityActivationGroup Group, UBOHGameplayAbility* BOHAbility)
{
	check(BOHAbility);
	check(ActivationGroupCounts[(uint8)Group] < INT32_MAX);

	ActivationGroupCounts[(uint8)Group]++;

	const bool bReplicateCancelAbility = false;

	switch (Group)
	{
	case EBOHAbilityActivationGroup::Independent:
		// Independent abilities do not cancel any other abilities.
		break;

	case EBOHAbilityActivationGroup::Exclusive_Replaceable:
	case EBOHAbilityActivationGroup::Exclusive_Blocking:
		CancelActivationGroupAbilities(EBOHAbilityActivationGroup::Exclusive_Replaceable, BOHAbility, bReplicateCancelAbility);
		break;

	default:
		checkf(false, TEXT("AddAbilityToActivationGroup: Invalid ActivationGroup [%d]\n"), (uint8)Group);
		break;
	}

	const int32 ExclusiveCount = ActivationGroupCounts[(uint8)EBOHAbilityActivationGroup::Exclusive_Replaceable] + ActivationGroupCounts[(uint8)EBOHAbilityActivationGroup::Exclusive_Blocking];
	if (!ensure(ExclusiveCount <= 1))
	{
		UE_LOG(LogBOHCoreAbilitySystem, Error, TEXT("AddAbilityToActivationGroup: Multiple exclusive abilities are running."));
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::RemoveAbilityFromActivationGroup(EBOHAbilityActivationGroup Group, UBOHGameplayAbility* BOHAbility)
{
	check(BOHAbility);
	check(ActivationGroupCounts[(uint8)Group] > 0);

	ActivationGroupCounts[(uint8)Group]--;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::CancelActivationGroupAbilities(EBOHAbilityActivationGroup Group, UBOHGameplayAbility* IgnoreBOHAbility, bool bReplicateCancelAbility)
{
	CancelAbilitiesByFunc
	(
		[this, Group, IgnoreBOHAbility](const UBOHGameplayAbility* BOHAbility, FGameplayAbilitySpecHandle Handle)
		{
			return ((BOHAbility->GetActivationGroup() == Group) && (BOHAbility != IgnoreBOHAbility));
		},
		bReplicateCancelAbility
	);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::AddDynamicTagGameplayEffect(const FGameplayTag& Tag)
{
	const TSubclassOf<UGameplayEffect> DynamicTagGE = UBOHAssetManager::GetSubclass(UBOHGameData::Get()->DynamicTagGameplayEffect);
	if (!DynamicTagGE)
	{
		UE_LOG(LogBOHCoreAbilitySystem, Warning, TEXT("AddDynamicTagGameplayEffect: Unable to find DynamicTagGameplayEffect [%s]."), *UBOHGameData::Get()->DynamicTagGameplayEffect.GetAssetName());
		return;
	}

	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(DynamicTagGE, 1.0f, MakeEffectContext());
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

	if (!Spec)
	{
		UE_LOG(LogBOHCoreAbilitySystem, Warning, TEXT("AddDynamicTagGameplayEffect: Unable to make outgoing spec for [%s]."), *BOHGetNameSafe(DynamicTagGE, true));
		return;
	}

	Spec->DynamicGrantedTags.AddTag(Tag);

	ApplyGameplayEffectSpecToSelf(*Spec);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag)
{
	const TSubclassOf<UGameplayEffect> DynamicTagGE = UBOHAssetManager::GetSubclass(UBOHGameData::Get()->DynamicTagGameplayEffect);
	if (!DynamicTagGE)
	{
		UE_LOG(LogBOHCoreAbilitySystem, Warning, TEXT("RemoveDynamicTagGameplayEffect: Unable to find gameplay effect [%s]."), *UBOHGameData::Get()->DynamicTagGameplayEffect.GetAssetName());
		return;
	}

	FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(Tag));
	Query.EffectDefinition = DynamicTagGE;

	RemoveActiveEffects(Query);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void UBOHAbilitySystemComponent::GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle)
{
	TSharedPtr<FAbilityReplicatedDataCache> ReplicatedData = AbilityTargetDataMap.Find(FGameplayAbilitySpecHandleAndPredictionKey(AbilityHandle, ActivationInfo.GetActivationPredictionKey()));
	if (ReplicatedData.IsValid())
	{
		OutTargetDataHandle = ReplicatedData->TargetData;
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

bool UBOHAbilitySystemComponent::HasActivatableTriggeredAbilityCheckingInstances(FGameplayTag Tag)
{
	//TODO: This method is a copy of the equivalent one at engine level but with bugs fixed, it should be removed
	//once the engine one gets fixed

	TArray<FGameplayAbilitySpec> Specs = GetActivatableAbilities();
	const FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	for (const FGameplayAbilitySpec& Spec : Specs)
	{
		UGameplayAbility* InstancedAbility = Spec.GetPrimaryInstance();
		UBOHGameplayAbility* const AbilitySource = Cast<UBOHGameplayAbility>(InstancedAbility ? InstancedAbility : Spec.Ability.Get());

		if (AbilitySource == nullptr)
		{
			continue;
		}

		const TArray<FAbilityTriggerData>& Triggers = AbilitySource->GetAbilityTriggers();
		for (const FAbilityTriggerData& Data : Triggers)
		{
			if (Data.TriggerTag == Tag && AbilitySource->CanActivateAbility(Spec.Handle, ActorInfo, nullptr, nullptr, nullptr))
			{
				return true;
			}
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

bool UBOHAbilitySystemComponent::GetInputTagFromAbilitySpec(const FGameplayAbilitySpec* Spec, FGameplayTag& OutTag)
{
	const UBOHInputTags& inputTags = UBOHInputTags::Get();
	for (const FGameplayTag& tag : Spec->DynamicAbilityTags)
	{
		if (tag.MatchesTag(inputTags.Input_Filter))
		{
			OutTag = tag;
			return true;
		}
	}

	return false;
}

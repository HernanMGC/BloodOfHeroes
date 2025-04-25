// Copyright Tequila Works S.L. 2022

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BOH/GAS/Attributes/BOHAttributeSet.h"
#include "BOH/GAS/BOHGameplayAbility.h"

#include "BOHAbilitySystemComponent.generated.h"

struct FGameplayTag;
struct FGameplayAbilitySpec;

class UBOHGameplayAbility;
class UBOHAbilityTagRelationshipMapping;

USTRUCT(BlueprintType)
struct FBOHAbilityActivationBufferData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAbilitySpecHandle AbilitySpecHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BufferStartTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BufferDuration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<const UBOHGameplayAbility> CDO = nullptr;

public:
	FBOHAbilityActivationBufferData() = default;

	FBOHAbilityActivationBufferData(const FGameplayAbilitySpecHandle InAbilitySpecHandle, const float InBufferStartTime, const float InBufferDuration, const UBOHGameplayAbility* InCDO)
		: AbilitySpecHandle(InAbilitySpecHandle)
		, BufferStartTime(InBufferStartTime)
		, BufferDuration(InBufferDuration)
		, CDO(InCDO)
	{
	}
	
	FORCEINLINE bool ShouldBeRemoved(const float CurrentTime) const { return RemainingLifeTime(CurrentTime) <= 0.0f; };
	FORCEINLINE float RemainingLifeTime(const float CurrentTime) const { return BufferDuration - (CurrentTime - BufferStartTime); };
	
	bool operator ==(FBOHAbilityActivationBufferData& Other) const
	{
		return AbilitySpecHandle == Other.AbilitySpecHandle;
	}

	bool operator !=(FBOHAbilityActivationBufferData& Other) const
	{
		return AbilitySpecHandle != Other.AbilitySpecHandle;
	}

	bool operator ==(const FBOHAbilityActivationBufferData& Other) const
	{
		return AbilitySpecHandle == Other.AbilitySpecHandle;
	}

	bool operator !=(const FBOHAbilityActivationBufferData& Other) const
	{
		return AbilitySpecHandle != Other.AbilitySpecHandle;
	}
};

/**
 * @class UBOHAbilitySystemComponent
 * @brief Base ability system component class used by this project.
 */
UCLASS(ClassGroup=AbilitySystem, hidecategories=(Object,LOD,Lighting,Transform,Sockets,TextureStreaming), editinlinenew, meta=(BlueprintSpawnableComponent))
class BOH_API UBOHAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FBOHAbilityInputConsumed, const FGameplayTag&);
	
	FBOHAbilityInputConsumed OnAbilityInputPressedConsumed;	// Triggered when an input press is consumed by the ASC. Either activation or press event.
	FBOHAbilityInputConsumed OnAbilityInputHeldConsumed;		// Triggered when an input hold is consumed by the ASC.
	FBOHAbilityInputConsumed OnAbilityInputReleasedConsumed; // Triggered when an input released is consumed by the ASC.

	DECLARE_MULTICAST_DELEGATE_OneParam(FBOHAbilityActivationBufferEvent, const FBOHAbilityActivationBufferData&);

	FBOHAbilityActivationBufferEvent OnAbilityActivationBuffered;
	FBOHAbilityActivationBufferEvent OnAbilityActivationBufferedExpired;
	FBOHAbilityActivationBufferEvent OnAbilityActivationBufferedConsumed;
	
public:
	UBOHAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;

#pragma region  UActorComponent
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
#pragma endregion

	virtual int32 HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload) override;

	bool CancelAbilityFromGameplayEvent
	(
		FGameplayAbilitySpecHandle Handle,
		FGameplayAbilityActorInfo* ActorInfo,
		FGameplayTag EventTag,
		const FGameplayEventData* Payload,
		UAbilitySystemComponent& Component
	);

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	typedef TFunctionRef<bool(const UBOHGameplayAbility* BOHAbility, FGameplayAbilitySpecHandle Handle)> TShouldCancelAbilityFunc;
	void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility);

	void CancelInputActivatedAbilities(bool bReplicateCancelAbility);

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

	bool IsActivationGroupBlocked(EBOHAbilityActivationGroup Group) const;
	void AddAbilityToActivationGroup(EBOHAbilityActivationGroup Group, UBOHGameplayAbility* BOHAbility);
	void RemoveAbilityFromActivationGroup(EBOHAbilityActivationGroup Group, UBOHGameplayAbility* BOHAbility);
	void CancelActivationGroupAbilities(EBOHAbilityActivationGroup Group, UBOHGameplayAbility* IgnoreBOHAbility, bool bReplicateCancelAbility);

	// Uses a gameplay effect to add the specified dynamic granted tag.
	void AddDynamicTagGameplayEffect(const FGameplayTag& Tag);

	// Removes all active instances of the gameplay effect that was used to add the specified dynamic granted tag.
	void RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag);

	/** Gets the ability target data associated with the given ability handle and activation info */
	void GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle);

	/** Sets the current tag relationship mapping, if null it will clear it out */
	void SetTagRelationshipMapping(UBOHAbilityTagRelationshipMapping* NewMapping);
	
	/** Looks at ability tags and gathers additional required and blocking tags */
	void GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const;

	/**
	 * Returns a reference to the Attribute Set instance, if one exists in this component
	 *
	 * @param AttributeSetClass The type of attribute set to look for
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Gameplay Attributes")
	UBOHAttributeSet* GetBOHAttributeSet(TSubclassOf<UBOHAttributeSet> AttributeSetClass) const;

	/** 
	  * Copy of UAbilitySystemComponent::HasActivatableTriggeredAbility that takes into account primary instanced ability
	  * Fixes issues with Abilities that are InstancedPerActor which seems to be an engine bug.
	  * TODO: This method is a copy of the equivalent one at engine level but with bugs fixed, it should be removed 
	  * once the engine one gets fixed
	  */
	bool HasActivatableTriggeredAbilityCheckingInstances(FGameplayTag Tag);

	static bool GetInputTagFromAbilitySpec(const FGameplayAbilitySpec* Spec, FGameplayTag& OutTag);

#pragma region Activation Buffer

	/**
	 * @brief Processes the Activation Buffer entries and can activate abilities if found to be valid.
	 * @param DeltaTime Time between frames.
	 * @param bGamePaused Is the game currently paused.
	 */
	virtual void ProcessAbilityActivationBuffer(float DeltaTime, bool bGamePaused);
	
	/**
	 * @brief Attempts to register the given Ability to the Activation Buffer.
	 * @param Ability Ability to be registered to buffer.
	 * @return True if registered correctly, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category = "BOH|Ability")
	bool RegisterAbilityActivationBuffer(const TSubclassOf<UBOHGameplayAbility> Ability);

	/**
	 * @brief Attempts to unregister the given Ability from the Activation Buffer.
	 * @param Ability Ability to be unregistered from buffer.
	 * @return True if unregistered correctly, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category = "BOH|Ability")
	bool UnregisterAbilityActivationBuffer(const TSubclassOf<UBOHGameplayAbility> Ability);
	
	/**
	 * @return The buffered activation spec handles array.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BOH|Ability")
	const TArray<FBOHAbilityActivationBufferData>& GetActivationBufferSpecHandles() const { return BufferedActivationsSpecHandles; }

	/**
	 * @brief Removes the given ability spec handle from the buffer.
	 * @param Handle The handle to the ability spec to be removed.
	 * @return True if the ability buffer was removed, false otherwise.
	 */
	bool ClearBufferBySpecHandle(const FGameplayAbilitySpecHandle Handle);

	/**
	 * @brief Removes the ability with the given tag from the buffer.
	 * @param Tags The tags of the abilities to be removed.
	 * @return True if any ability buffer was removed, false otherwise.
	 */
	bool ClearBufferByAbilityTags(FGameplayTagContainer Tags);

	/**
	 * @brief Clears the Activation Buffer.
	 * @return The number of abilities removed from the buffer.
	 */
	int32 ClearBuffer();

	/** Returns true if any passed in tags are blocked for buffering */
	virtual bool IsBufferBlockedForAbility(const FGameplayTagContainer& Tags) const;

	/** Block or cancel blocking for specific ability buffer tags */
	void BlockBuffersWithTags(const FGameplayTagContainer& Tags);
	void UnBlockBuffersWithTags(const FGameplayTagContainer& Tags);
	void CancelBuffersWithTags(const FGameplayTagContainer& Tags);

	FGameplayTagContainer GetBufferBlockingTags() const { return BlockedBufferTags.GetExplicitGameplayTags(); }
#pragma endregion

protected:
	void TryActivateAbilitiesOnSpawn();

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	virtual void NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability) override;
	virtual void NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason) override;
	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;
	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;
	virtual void HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bCanBeCanceled) override;

	/** Notify client that an ability failed to activate */
	UFUNCTION(Client, Unreliable)
	void ClientNotifyAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);

	void HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);
	
protected:
	// If set, this table is used to look up tag relationships for activate and cancel
	UPROPERTY()
	TObjectPtr<UBOHAbilityTagRelationshipMapping> TagRelationshipMapping;

	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// Handles to abilities that had their input released this frame.
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	// Handles to abilities that have their input held.
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	// Handles to abilities that have been buffered for activation.
	TArray<FBOHAbilityActivationBufferData> BufferedActivationsSpecHandles;

	// Number of abilities running in each activation group.
	int32 ActivationGroupCounts[(uint8)EBOHAbilityActivationGroup::MAX];

#pragma region Activation Buffer
	// The current buffer blocking tags.
	FGameplayTagCountContainer BlockedBufferTags;
#pragma endregion

	/** Abilities that are triggered from a gameplay event */
	TMap<FGameplayTag, TArray<FGameplayAbilitySpecHandle>> GameplayEventCancelledAbilities;

	/** Abilities that are triggered from a tag being added to the owner */
	TMap<FGameplayTag, TArray<FGameplayAbilitySpecHandle>> OwnedTagCancelledAbilities;
};

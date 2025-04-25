// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Containers/Array.h"
#include "Containers/Map.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Templates/SubclassOf.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "BOHGameplayAbility.generated.h"

class UBOHCameraMode;
class AActor;
class AController;
class ABOHCharacter;
class ABOHPlayerController;
class APlayerController;
class FText;
class IBOHAbilitySourceInterface;
class UAnimMontage;
class UBOHAbilityCost;
class UBOHAbilitySystemComponent;
class UBOHHeroComponent;
class UObject;
class UBOHPlayerPawnComponent;

struct FFrame;
struct FGameplayAbilityActorInfo;
struct FGameplayEffectSpec;
struct FGameplayEventData;

/**
 * @brief Defines how an ability is meant to activate.
 */
UENUM(BlueprintType)
enum class EBOHAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn,

	// Activate only using trigger events.
	OnEventTriggered,
};

/**
 * @brief Defines how an ability activates in relation to other abilities.
 */
UENUM(BlueprintType)
enum class EBOHAbilityActivationGroup : uint8
{
	// Ability runs independently of all other abilities.
	Independent,

	// Ability is canceled and replaced by other exclusive abilities.
	Exclusive_Replaceable,

	// Ability blocks all other exclusive abilities from activating.
	Exclusive_Blocking,

	MAX	UMETA(Hidden)
};

/**
 * @brief Failure reason that can be used to play an animation montage when a failure occurs
 */
USTRUCT(BlueprintType)
struct FBOHAbilityMontageFailureMessage
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	// All the reasons why this ability has failed
	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer FailureTags;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAnimMontage> FailureMontage = nullptr;
};

/**
 * UBOHGameplayAbility
 *
 *	The base gameplay ability class used by this project.
 */
UCLASS(Abstract, HideCategories = Input, Meta = (ShortTooltip = "The base gameplay ability class used by this project."))
class BOH_API UBOHGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
	friend class UBOHAbilitySystemComponent;

public:
	UBOHGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "BOH|Ability")
	UBOHAbilitySystemComponent* GetBOHAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "BOH|Ability")
	ABOHPlayerController* GetBOHPlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "BOH|Ability")
	AController* GetControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "BOH|Ability")
	ABOHCharacter* GetBOHCharacterFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "BOH|Ability")
	UBOHPlayerPawnComponent* GetPlayerPawnComponentFromActorInfo() const;

	EBOHAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	EBOHAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }

#pragma region Camera

	// Sets the ability's camera mode.
	UFUNCTION(BlueprintCallable, Category = "BOH|Ability")
	void SetCameraMode(TSubclassOf<UBOHCameraMode> CameraMode);

	// Clears the ability's camera mode.  Automatically called if needed when the ability ends.
	UFUNCTION(BlueprintCallable, Category = "BOH|Ability")
	void ClearCameraMode();

#pragma endregion	


#pragma region Activation Buffer
	virtual bool ShouldBeBuffered() const { return BufferAbilityActivation && BufferDuration > 0.0f; }
	virtual bool DoesAbilitySatisfyBufferTagRequirements(const UBOHAbilitySystemComponent& ASC, OUT FGameplayTagContainer* OptionalRelevantTags) const;
#pragma endregion

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

	// Returns true if the requested activation group is a valid transition.
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "BOH|Ability", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool CanChangeActivationGroup(EBOHAbilityActivationGroup NewGroup) const;

	// Tries to change the activation group.  Returns true if it successfully changed.
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "BOH|Ability", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool ChangeActivationGroup(EBOHAbilityActivationGroup NewGroup);

	void OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
	{
		NativeOnAbilityFailedToActivate(FailedReason);
		ScriptOnAbilityFailedToActivate(FailedReason);
	}

	FORCEINLINE const TArray<FAbilityTriggerData>& GetAbilityTriggers() const { return AbilityTriggers; }
	FORCEINLINE const TArray<FAbilityTriggerData>& GetAbilityCancelTriggers() const { return AbilityCancelTriggers; } 

protected:
	// Called when the ability fails to activate
	virtual void NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;

	// Called when the ability fails to activate
	UFUNCTION(BlueprintImplementableEvent)
	void ScriptOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;

	//~UGameplayAbility interface
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void SetCanBeCanceled(bool bCanBeCanceled) override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual FGameplayEffectContextHandle MakeEffectContext(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const override;
	virtual void ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec, FGameplayAbilitySpec* AbilitySpec) const override;
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const override;
	//~End of UGameplayAbility interface

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnPawnAvatarSet();

	virtual void GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, float& OutSourceLevel, const IBOHAbilitySourceInterface*& OutAbilitySource, AActor*& OutEffectCauser) const;

	/** Called when this ability is granted to the ability system component. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityAdded")
	void K2_OnAbilityAdded();

	/** Called when this ability is removed from the ability system component. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityRemoved")
	void K2_OnAbilityRemoved();

	/** Called when the ability system is initialized with a pawn avatar. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnPawnAvatarSet")
	void K2_OnPawnAvatarSet();

protected:
	// Cancel Ability Event Triggers.
	UPROPERTY(EditDefaultsOnly, Category = "Triggers")
	TArray<FAbilityTriggerData> AbilityCancelTriggers;
	
	// Should abilities related to this ability be fed to active ability instances as press or release events.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Ability Activation")
	bool FeedInputToInstances = true;

	// Defines how this ability is meant to activate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Ability Activation")
	EBOHAbilityActivationPolicy ActivationPolicy;

	// Defines the relationship between this ability activating and other abilities activating.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Ability Activation")
	EBOHAbilityActivationGroup ActivationGroup;

#pragma region Buffer
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Ability Buffering")
	bool BufferAbilityActivation = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Ability Buffering", meta=(EditCondition="BufferAbilityActivation", EditConditionHides, UIMin=0.0f))
	float BufferDuration = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Ability Buffering", meta=(EditCondition="BufferAbilityActivation", EditConditionHides))
	FGameplayTagContainer CancelBuffersWithTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Ability Buffering", meta=(EditCondition="BufferAbilityActivation", EditConditionHides))
	FGameplayTagContainer BlockBuffersWithTags;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Ability Buffering", meta=(EditCondition="BufferAbilityActivation", EditConditionHides))
	FGameplayTagContainer BufferBlockingTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Ability Buffering", meta=(EditCondition="BufferAbilityActivation", EditConditionHides))
	FGameplayTagContainer BufferRequiredTags;
#pragma endregion

	// Additional costs that must be paid to activate this ability
	UPROPERTY(EditDefaultsOnly, Instanced, Category = Costs)
	TArray<TObjectPtr<UBOHAbilityCost>> AdditionalCosts;

	// Map of failure tags to simple error messages
	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
	TMap<FGameplayTag, FText> FailureTagToUserFacingMessages;

	// Map of failure tags to anim montages that should be played with them
	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> FailureTagToAnimMontage;

	// If true, extra information should be logged when this ability is canceled. This is temporary, used for tracking a bug.
	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
	bool bLogCancelation;

	// Current camera mode set by the ability.
	TSubclassOf<UBOHCameraMode> ActiveCameraMode;	
};

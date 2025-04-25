// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "Components/PawnComponent.h"

// BOH
#include "BOHPawnExtensionComponent.generated.h"

//// ForwardDeclarations
// BOH
class UBOHAbilitySystemComponent;
class UBOHPawnData;

/**
 * Component that adds functionality to all Pawn classes so it can be used for characters/vehicles/etc. This coordinates
 * the initialization of other components.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BOH_API UBOHPawnExtensionComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	// The name of this overall feature, this one depends on the other named component features.
	static const FName NAME_ActorFeatureName;

public:
	UBOHPawnExtensionComponent(const FObjectInitializer& ObjectInitializer);

	/**
	 * @brief Returns the pawn extension component if one exists on the specified actor.
	 */
	UFUNCTION(BlueprintPure, Category = "BOH|Pawn")
	static UBOHPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UBOHPawnExtensionComponent>() : nullptr); }

	/**
	 * @brief Gets the pawn data, which is used to specify pawn properties in data
	 */
	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	/**
	 * @brief Sets the current pawn data, which is used to specify pawn properties in data.
	 */
	void SetPawnData(const UBOHPawnData* InPawnData);

	/**
	 * @brief Gets the current ability system component, which may be owned by a different actor.
	 */
	UFUNCTION(BlueprintPure, Category = "BOH|Pawn")
	UBOHAbilitySystemComponent* GetBOHAbilitySystemComponent() const { return AbilitySystemComponent; }

	/**
	 * @brief Should be called by the owning pawn to become the avatar of the ability system.
	 */
	void InitializeAbilitySystem(UBOHAbilitySystemComponent* InASC, AActor* InOwnerActor);

	/**
	 * @brief Should be called by the owning pawn to remove itself as the avatar of the ability system.
	 */
	void UninitializeAbilitySystem();

	/**
	 * @brief Should be called by the owning pawn when the pawn's controller changes.
	 */
	void HandleControllerChanged();

	/**
	 * @brief Should be called by the owning pawn when the player state has been replicated.
	 */
	void HandlePlayerStateReplicated();

	/**
	 * @brief Should be called by the owning pawn when the input component is setup.
	 */
	void SetupPlayerInputComponent();

	/**
	 * @brief Register with the OnAbilitySystemInitialized delegate and broadcast if our pawn has been registered with the ability system component
	 */
	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);

	/**
	 * @brief Register with the OnAbilitySystemUninitialized delegate fired when our pawn is removed as the ability system's avatar actor.
	 */
	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	
	UFUNCTION()
	void OnRep_PawnData();

	// Delegate fired when our pawn becomes the ability system's avatar actor.
	FSimpleMulticastDelegate OnAbilitySystemInitialized;

	// Delegate fired when our pawn is removed as the ability system's avatar actor.
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;

	// Pawn data used to create the pawn. Specified from a spawn function or on a placed instance.
	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_PawnData, Category = "BOH|Pawn")
	TObjectPtr<const UBOHPawnData> PawnData = nullptr;

	// Pointer to the ability system component that is cached for convenience.
	UPROPERTY()
	TObjectPtr<UBOHAbilitySystemComponent> AbilitySystemComponent = nullptr;
};

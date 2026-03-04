// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayTagContainer.h"

// BOH
#include "BOH/Characters/BOHUnit.h"
#include "BOH/Messages/BOHGameplayMessage.h"
#include "BOH/UI/MVVM/Models/BOHBaseViewModel.h"
#include "BOHVMPlayerUnits.generated.h"

//// ForwardDeclarations
// BOH
class ABOHUnit;
class ABOHPlayerController;

/**
 * View model for unit info visualization.
 */
UCLASS(BlueprintType)
class BOH_API UBOHVMPlayerUnits : public UBOHBaseViewModel
{
	GENERATED_BODY()

public:
	// Overriden to: bind events to the proper messages to refresh the view model.
	virtual void Init(FBOHViewModelInitParams& InitParams) override;

	// Overriden to: unbind events.
	virtual void Deinit() const override;
	
	/**
	 * On Unit selected message receive set the selected unit to cause the notification cascade.
	 * @param GameplayTag 
	 * @param SelectedUnitMessage 
	 */
	void OnUnitSelected(FGameplayTag GameplayTag, const FBOHSelectedUnitMessage& SelectedUnitMessage);

	/**
	 * Set selected unit info and call for MVVM field update.
	 * @param InUnit 
	 */
	void SetSelectedUnit(ABOHUnit* InUnit);

	/**
	* Returns whether there is a selected unit for field notify.
	*/
	UFUNCTION(BlueprintPure, FieldNotify)
	bool GetIsUnitSelected() const;
	
	/**
	* Returns UnitID for field notify.
	*/
	UFUNCTION(BlueprintPure, FieldNotify)
	int32 GetUnitID() const;

	/**
	* Returns TeamID for field notify.
	*/
	UFUNCTION(BlueprintPure, FieldNotify)
	int32 GetTeamID() const;

	/**
	* Returns ype for field notify.
	*/
	UFUNCTION(BlueprintPure, FieldNotify)
	EBOHUnitType GetUnitType() const;

	/**
	* Returns Speed for field notify.
	*/
	UFUNCTION(BlueprintPure, FieldNotify)
	float GetSpeed() const;

	/**
	* Returns formatted Speed for field notify.
	*/
	UFUNCTION(BlueprintPure, FieldNotify)
	FText GetSpeedFormatted() const;

	/**
	* Returns EvasionRadius for field notify.
	*/
	UFUNCTION(BlueprintPure, FieldNotify)
	float GetEvasionRadius() const;

	/**
	* Returns formatted EvasionRadius for field notify.
	*/
	UFUNCTION(BlueprintPure, FieldNotify)
	FText GetEvasionRadiusFormatted() const;

	/**
	* Returns ReachRadius for field notify.
	*/
	UFUNCTION(BlueprintPure, FieldNotify)
	float GetReachRadius() const;

	/**
	* Returns formatted ReachRadius for field notify.
	*/
	UFUNCTION(BlueprintPure, FieldNotify)
	FText GetReachRadiusFormatted() const;

private:
	// Selected unit ptr.
	UPROPERTY(Transient)
	TObjectPtr<ABOHUnit> SelectedUnit = nullptr;

	// Owning player controller. TODO: Consider moving this to the base class.
	UPROPERTY(Transient)
	TObjectPtr<ABOHPlayerController> OwningPlayerController = nullptr;

	// On unit selected message listener handle.
	FGameplayMessageListenerHandle OnUnitSelectedMessageListenerHandle;
};

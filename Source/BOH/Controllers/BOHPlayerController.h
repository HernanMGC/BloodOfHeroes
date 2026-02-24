// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"

// BOH
#include "BOH/Component/Path/BOHPathPointActor.h"
#include "BOHPlayerController.generated.h"

//// ForwardDeclarations
// UnrealEngine
class UInputMappingContext;
class UInputAction;
class UNiagaraSystem;

// BOH
class ABOHUnit;

DECLARE_LOG_CATEGORY_EXTERN(LogBOPlayerController, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitSelected, ABOHPlayerController*, PlayerController, ABOHUnit*, SelectedUnit);

/**
 * Player controller for Blood of Heroes. Allows to select units and send them orders.
 */
UCLASS(Abstract)
class BOH_API ABOHPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// Default mapping context for player controller.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;
		
	// Click input Action.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SelectActorAction = nullptr;

	// Delete actor input Action.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> DeleteActorAction = nullptr;

	// Move input action.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction = nullptr;

	// Zoom input action.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ZoomAction = nullptr;

	// FX Class that we will spawn when clicking.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Input")
	TObjectPtr<UNiagaraSystem> FXCursor = nullptr;

	// On unit selected delegate.
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable, Category = "BOH|Input")
	FOnUnitSelected OnUnitSelected;

	// Double click time threshold.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Input")
	float DoubleClickTimeThreshold = 0.05f;

	// Drag time threshold.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Input")
	float DragTimeThreshold = 0.01f;

public:
	/**
	 * Constructor. Tick removed. Basic initialization for show cursor.
	 */
	ABOHPlayerController();

	// Overriden to: add replicated variables.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifeProps) const override;
	
	/**
	 * Returns selected unit.
	 * @return 
	 */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE ABOHUnit* GetSelectedUnit() const { return SelectedUnit; }

	/**
	 * Spawn units at given UnitStartPointsTransforms.This is expected to be called only on server.
	 * @param UnitStartPointsTransforms 
	 */
	void SpawnUnits(TArray<FTransform> UnitStartPointsTransforms);

protected:
	// Overriden to: TODO.
	virtual void BeginPlay() override;

	// Overriden to: Unbind events and invalidate timers.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Overriden to: Update drag time.
	virtual void Tick(float DeltaSeconds) override;
	
	// Overriden to: Bind click input events.
	virtual void SetupInputComponent() override;

#pragma region ActorActions
	/** Input handlers for SelectActor action. */
	void OnSelectActorInputStarted();
	void OnSelectActorTriggered();
	void OnSelectActorReleased();
	
	/** Input handlers for DeleteActor action. */
	void OnDeleteActorTriggered();

	/**
	 * Input handlers for Move action. 
	 * @param Value 
	 */
	void OnMoveInputTriggered(const FInputActionValue& Value);

	/**
	 * Input handlers for Zoom action.
	 * @param Value 
	 */
	void OnZoomInputTriggered(const FInputActionValue& Value);
#pragma endregion // SelectActorAction

private:
	/**
	 * Set selected unit.
	 * @param Unit 
	 */
	void SetSelectedUnit(ABOHUnit* Unit);

	/**
	 * Set selected path actor.
	 * @param PathActor 
	 */
	void SetSelectedPathActor(ABOHPathActor* PathActor);

	/**
	 * Handle press event.
	 */
	void HandleSingleClick(const FHitResult& Hit);

	
	/**
	 * Handle press event.
	 */
	void HandleDoubleClick(const FHitResult& Hit);

	/**
	 * Double click timer
	 */
	void OnDoubleClickTimerFinished();

protected:
	// Last hit actor.
	UPROPERTY(Transient)
	TObjectPtr<AActor> LastHitActor = nullptr;
	
	// Currently selected unit.
	UPROPERTY(Transient)
	TObjectPtr<ABOHUnit> SelectedUnit = nullptr;

	// Currently selected unit path point unit.
	UPROPERTY(Transient)
	TObjectPtr<ABOHPathActor> SelectedUnitPathActor = nullptr;

	// List of player's unit
	UPROPERTY(Replicated, Transient)
	TArray<TObjectPtr<ABOHUnit>> PlayerUnits;

	// Input is being pressed.
	bool bIsPressing = false;

	// Dragging time.
	float DraggingTime = 0.f;
	
	// Input is in double click threshold.
	bool bIsInDoubleClickThreshold = false;

	// Double click timer handle.
	FTimerHandle DoubleClickTimerHandle;
};


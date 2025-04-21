// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "BloodOfHeroes/Component/Path/BOHPathPointActor.h"
#include "GameFramework/PlayerController.h"

// BOH
#include "BOHPlayerController.generated.h"

class UBOHHudWidget;
//// ForwardDeclaration
// UnrealEngine
class UInputMappingContext;
class UInputAction;
class UNiagaraSystem;

// BOH
class ABOHCharacter;

DECLARE_LOG_CATEGORY_EXTERN(LogBOPlayerController, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitSelected, ABOHPlayerController*, PlayerController, ABOHCharacter*, SelectedUnit);

/**
 * Player controller for Blood of Heroes. Allows to select characters and send them orders.
 */
UCLASS()
class BLOODOFHEROES_API ABOHPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// Default mapping context for player controller.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;
		
	// Click input Action.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SelectActorAction = nullptr;

	// Click input Action.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> DeleteActorAction = nullptr;

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

	// HUD Widget class.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|UI")
	TSubclassOf<UBOHHudWidget> HUDWidgetClass = nullptr;

public:
	/**
	 * Constructor. Tick removed. Basic initialization for show cursor.
	 */
	ABOHPlayerController();

	/**
	 * Returns selected unit.
	 * @return 
	 */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE ABOHCharacter* GetSelectedUnit() const { return SelectedUnit; }

protected:
	// Overriden to: Add HUD to viewport.
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
	void OnDeleteActorInputStarted();
	void OnDeleteActorTriggered();
	void OnDeleteActorReleased();
#pragma endregion // SelectActorAction

private:
	/**
	 * Set selected unit.
	 * @param Unit 
	 */
	void SetSelectedUnit(ABOHCharacter* Unit);

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
	TObjectPtr<AActor> LastHiActor = nullptr;
	
	// Currently selected character.
	UPROPERTY(Transient)
	TObjectPtr<ABOHCharacter> SelectedUnit = nullptr;

	// Currently selected unit path point character.
	UPROPERTY(Transient)
	TObjectPtr<ABOHPathActor> SelectedUnitPathActor = nullptr;

	// HUD Widget reference.
	TObjectPtr<UBOHHudWidget> HUDWidget = nullptr; 

	// Input is being pressed.
	bool bIsPressing = false;

	// Dragging time.
	float DraggingTime = 0.f;
	
	// Input is in double click threshold.
	bool bIsInDoubleClickThreshold = false;

	// Double click timer handle.
	FTimerHandle DoubleClickTimerHandle;
};


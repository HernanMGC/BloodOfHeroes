// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
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
	TObjectPtr<UInputAction> SelectUnitAction = nullptr;

	// FX Class that we will spawn when clicking.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|Input")
	TObjectPtr<UNiagaraSystem> FXCursor = nullptr;

	// HUD Widget class.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|UI")
	TSubclassOf<UBOHHudWidget> HUDWidgetClass = nullptr;

	// On unit selected delegate.
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnUnitSelected OnUnitSelected;

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
	
	// Overriden to: Bind click input events.
	virtual void SetupInputComponent() override;

#pragma region SelectUnitAction
	/** Input handlers for SelectUnit action. */
	void OnInputStarted();
	void OnSelectUnitTriggered();
	void OnSelectUnitReleased();
#pragma endregion // SelectUnitAction

private:
	/**
	 * Set selected unit.
	 * @param Unit 
	 */
	void SetSelectedUnit(ABOHCharacter* Unit);

protected:
	// Currently selected character.
	UPROPERTY(Transient)
	TObjectPtr<ABOHCharacter> SelectedUnit = nullptr;

private:
	// HUD Widget reference.
	TObjectPtr<UBOHHudWidget> HUDWidget = nullptr; 
};

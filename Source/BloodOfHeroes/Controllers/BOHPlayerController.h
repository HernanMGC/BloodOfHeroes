// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

// BOH
#include "BOHPlayerController.generated.h"

//// ForwardDeclaration
// UnrealEngine
class UInputMappingContext;
class UInputAction;
class UNiagaraSystem;

// BOH
class ABOHCharacter;

DECLARE_LOG_CATEGORY_EXTERN(LogBOPlayerController, Log, All);

/**
 * Player controller for Blood of Heroes. Allows to select characters and send them orders.
 */
UCLASS()
class BLOODOFHEROES_API ABOHPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// Default mapping context for player controller.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BOH|Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;
		
	// Click input Action.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BOH|Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SelectUnitAction = nullptr;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BOH|Input")
	TObjectPtr<UNiagaraSystem> FXCursor = nullptr;
	
public:
	/**
	 * Constructor. Basic initialization for show cursor.
	 */
	ABOHPlayerController();

protected:
	// Overriden to: Bind click input events.
	virtual void SetupInputComponent() override;

#pragma region SelectUnitAction
	/** Input handlers for SelectUnit action. */
	void OnInputStarted();
	void OnSelectUnitTriggered();
	void OnSelectUnitReleased();
#pragma endregion // SelectUnitAction
	
protected:
	// Currently selected character.
	UPROPERTY(Transient)
	TObjectPtr<ABOHCharacter> SelectedCharacter = nullptr;

private:
	// Cached last hit location.
	TOptional<FVector> CachedLastHitLocation;;
};

// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHPlayerController.h"

// UnrealEngine
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraFunctionLibrary.h"
#include "Blueprint/UserWidget.h"

// BOH
#include "BloodOfHeroes/Characters/BOHCharacter.h"
#include "BloodOfHeroes/Component/Path/BOHPathPointActor.h"
#include "BloodOfHeroes/Component/Path/BOHUnitPathComponent.h"
#include "BloodOfHeroes/UI/BOHHudWidget.h"

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

DEFINE_LOG_CATEGORY(LogBOPlayerController);

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

ABOHPlayerController::ABOHPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	HUDWidget = CreateWidget<UBOHHudWidget>(this, HUDWidgetClass, TEXT("HUD"));
	if (!HUDWidget)
	{
		return;
	}
	HUDWidget->AddToViewport(0);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SelectUnitAction, ETriggerEvent::Started, this, &ThisClass::OnInputStarted);
		EnhancedInputComponent->BindAction(SelectUnitAction, ETriggerEvent::Triggered, this,
		                                   &ThisClass::OnSelectUnitTriggered);
		EnhancedInputComponent->BindAction(SelectUnitAction, ETriggerEvent::Completed, this,
		                                   &ThisClass::OnSelectUnitReleased);
		EnhancedInputComponent->BindAction(SelectUnitAction, ETriggerEvent::Canceled, this,
		                                   &ThisClass::OnSelectUnitReleased);
	}
	else
	{
		UE_LOG(LogBOPlayerController, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::OnInputStarted()
{
	UE_LOG(LogBOPlayerController, Display, TEXT("ABOHPlayerController::OnInputStarted"));
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::OnSelectUnitTriggered()
{
	if (bIsPressing)
	{
		return;
	}
	
	HandlePress();
	
	bIsPressing = true;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::OnSelectUnitReleased()
{
	bIsPressing = false;
	if (SelectedUnitPathPoint == nullptr)
	{
		return;
	}
	TObjectPtr<ABOHPathPointActor> TempSelectedUnitPathPoint = SelectedUnitPathPoint;
	SelectedUnitPathPoint = nullptr;
	
	UBOHUnitPathComponent* PathComp = SelectedUnit && TempSelectedUnitPathPoint ? SelectedUnit->GetComponentByClass<UBOHUnitPathComponent>() : nullptr;
	if (!PathComp)
	{
		return;
	}

	FHitResult Hit;
	if (const bool bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit))
	{
		PathComp->ModifyPointFromPath(TempSelectedUnitPathPoint->GetPathPointIndex(), Hit.Location);
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::SetSelectedUnit(ABOHCharacter* Unit)
{
	if (SelectedUnit && SelectedUnit != Unit)
	{
		SelectedUnit->SetIsUnitSelected(false);
	}

	if (Unit)
	{
		SelectedUnit = Unit;
		OnUnitSelected.Broadcast(this, Unit);
		SelectedUnit->SetIsUnitSelected(true);	
	}
}


////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::HandlePress()
{
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	const bool bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	
	// If we hit a surface, cache the location
	if (!bHitSuccessful)
	{
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, Hit.Location,
												   FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true,
												   ENCPoolMethod::None, true);

	ABOHCharacter* HitCharacter = Cast<ABOHCharacter>(Hit.GetActor());
	if (HitCharacter)
	{
		SetSelectedUnit(HitCharacter);
		UE_LOG(LogPlayerController, Display, TEXT("Character clicked and selected: %s"), *SelectedUnit->GetUnitInfo().ToString());
		return;
	}

	ABOHPathPointActor* HitPathPoint = Cast<ABOHPathPointActor>(Hit.GetActor());
	if (HitPathPoint && HitPathPoint->GetOwner() == SelectedUnit)
	{
		SelectedUnitPathPoint = HitPathPoint;
		return;
	}

	UBOHUnitPathComponent* PathComp = SelectedUnit ? SelectedUnit->GetComponentByClass<UBOHUnitPathComponent>() : nullptr;
	if (PathComp)
	{
		PathComp->AppendPointToPath(Hit.Location);
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

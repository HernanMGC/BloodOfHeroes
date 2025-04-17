// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHPlayerController.h"

// UnrealEngine
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraFunctionLibrary.h"

// BOH
#include "BloodOfHeroes/Characters/BOHCharacter.h"

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

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::OnSelectUnitTriggered()
{
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	
	
	// If we hit a surface, cache the location
	if (!bHitSuccessful)
	{
		return;
	}

	CachedLastHitLocation = Hit.Location;

	ABOHCharacter* HitCharacter = Cast<ABOHCharacter>(Hit.GetActor());
	if (!HitCharacter)
	{
		return;
	}

	UE_LOG(LogPlayerController, Display, TEXT("Character clicked and selected: %s"), *HitCharacter->GetUnitInfo().ToString());
	SelectedCharacter = HitCharacter;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::OnSelectUnitReleased()
{
	if (CachedLastHitLocation.IsSet())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedLastHitLocation.GetValue(),
		                                               FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true,
		                                               ENCPoolMethod::None, true);
	}
}
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
#include "BloodOfHeroes/Component/Path/BOHPathLineActor.h"
#include "BloodOfHeroes/Component/Path/BOHPathPointActor.h"
#include "BloodOfHeroes/Component/Path/BOHUnitPathComponent.h"
#include "BloodOfHeroes/Pawns/BOHPlayerPawn.h"
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

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (DoubleClickTimerHandle.IsValid())
	{
		DoubleClickTimerHandle.Invalidate();
	}
	
	Super::EndPlay(EndPlayReason);
}

void ABOHPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DraggingTime += DeltaSeconds;
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
		EnhancedInputComponent->BindAction(SelectActorAction, ETriggerEvent::Started, this, &ThisClass::OnSelectActorInputStarted);
		EnhancedInputComponent->BindAction(SelectActorAction, ETriggerEvent::Triggered, this,
		                                   &ThisClass::OnSelectActorTriggered);
		EnhancedInputComponent->BindAction(SelectActorAction, ETriggerEvent::Completed, this,
		                                   &ThisClass::OnSelectActorReleased);
		EnhancedInputComponent->BindAction(SelectActorAction, ETriggerEvent::Canceled, this,
		                                   &ThisClass::OnSelectActorReleased);

		
		// Setup mouse input events
		EnhancedInputComponent->BindAction(DeleteActorAction, ETriggerEvent::Triggered, this,
										   &ThisClass::OnDeleteActorTriggered);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::OnMoveInputTriggered);

		
		// Zooming
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ThisClass::OnZoomInputTriggered);
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

void ABOHPlayerController::OnSelectActorInputStarted()
{
	UE_LOG(LogBOPlayerController, Display, TEXT("ABOHPlayerController::OnInputStarted"));
	DraggingTime = 0.f;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::OnSelectActorTriggered()
{
	if (bIsPressing)
	{
		return;
	}
	
	// We look for the location in the world where the player has pressed the input
	UWorld* World = GetWorld();
	FHitResult Hit;
	const bool bHitSuccessful = World ? GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit) : false;
	
	if (!bHitSuccessful)
	{
		return;
	}

	if (bIsInDoubleClickThreshold && LastHiActor == Hit.GetActor())
	{
		HandleDoubleClick(Hit);
		bIsInDoubleClickThreshold = false;
		DoubleClickTimerHandle.Invalidate();
	}
	else
	{
		HandleSingleClick(Hit);
		bIsInDoubleClickThreshold = true;
		World->GetTimerManager().SetTimer(DoubleClickTimerHandle, this, &ThisClass::OnDoubleClickTimerFinished, DoubleClickTimeThreshold);
	}

	LastHiActor = Hit.GetActor();
	bIsPressing = true;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::OnSelectActorReleased()
{
	bIsPressing = false;

	bool bIsDragging = DraggingTime >= DragTimeThreshold;
	ABOHPathPointActor* PointActor = bIsDragging && SelectedUnit && SelectedUnitPathActor ? Cast<ABOHPathPointActor>(SelectedUnitPathActor) : nullptr;
	UBOHUnitPathComponent* PathComp = PointActor && PointActor->GetOwner() == SelectedUnit ? SelectedUnit->GetComponentByClass<UBOHUnitPathComponent>() : nullptr;
	if (!PathComp)
	{
		return;
	}

	FHitResult Hit;
	if (const bool bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit))
	{
		PathComp->ModifyPointFromPath(PointActor->GetPathPointIndex(), Hit.Location);
	}
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::OnDeleteActorTriggered()
{
	UBOHUnitPathComponent* PathComp = SelectedUnit ? SelectedUnit->GetComponentByClass<UBOHUnitPathComponent>() : nullptr;
	if (!SelectedUnitPathActor || SelectedUnitPathActor->GetOwner() != SelectedUnit || !PathComp)
	{
		return;
	}

	PathComp->RemovePointFromPath(SelectedUnitPathActor->GetPathPointIndex());
	PathComp->UpdatePathActors();
	SetSelectedPathActor(nullptr);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::OnMoveInputTriggered(const FInputActionValue& Value)
{
	ABOHPlayerPawn* PlayerPawn = Cast<ABOHPlayerPawn>(GetPawn());
	if (!PlayerPawn)
	{
		return;
	}
	
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// find out which way is forward
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// add movement
	PlayerPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
	PlayerPawn->AddMovementInput(RightDirection, MovementVector.X);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::OnZoomInputTriggered(const FInputActionValue& Value)
{
	
	ABOHPlayerPawn* PlayerPawn = Cast<ABOHPlayerPawn>(GetPawn());
	if (!PlayerPawn)
	{
		return;
	}

	// input is a Vector2D
	float ZoomAmount = Value.Get<float>();

	PlayerPawn->AddCameraBoomArmLegth(ZoomAmount);
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

	SelectedUnit = Unit;
	OnUnitSelected.Broadcast(this, Unit);

	if (Unit)
	{
		SelectedUnit->SetIsUnitSelected(true);	
	}
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::SetSelectedPathActor(ABOHPathActor* PathActor)
{
	if (SelectedUnitPathActor && SelectedUnitPathActor != PathActor)
	{
		SelectedUnitPathActor->SetIsPathActorSelected(false);
	}

	SelectedUnitPathActor = PathActor;
	if (PathActor)
	{
		SelectedUnitPathActor->SetIsPathActorSelected(true);	
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::HandleSingleClick(const FHitResult& Hit)
{
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

	ABOHPathActor* HitPathActor = Cast<ABOHPathActor>(Hit.GetActor());
	if (HitPathActor && HitPathActor->GetOwner() == SelectedUnit)
	{
		if (HitPathActor->CanBeEdit())
		{
			SetSelectedPathActor(HitPathActor);
		}
		return;
	}

	UBOHUnitPathComponent* PathComp = SelectedUnit ? SelectedUnit->GetComponentByClass<UBOHUnitPathComponent>() : nullptr;
	if (PathComp)
	{
		PathComp->AppendPointToPath(Hit.Location);
		SetSelectedPathActor(nullptr);
	}
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::HandleDoubleClick(const FHitResult& Hit)
{
	ABOHPathLineActor* HitPathLine = Cast<ABOHPathLineActor>(Hit.GetActor());
	if (!HitPathLine || !HitPathLine->CanBeEdit() || HitPathLine->GetOwner() != SelectedUnit)
	{
		return;
	}

	UBOHUnitPathComponent* PathComp = SelectedUnit ? SelectedUnit->GetComponentByClass<UBOHUnitPathComponent>() : nullptr;
	if (!PathComp)
	{
		return;
	}

	PathComp->AddPointToPath(Hit.Location, HitPathLine->GetPathPointIndex());
	PathComp->UpdatePathActors();
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::OnDoubleClickTimerFinished()
{
	bIsInDoubleClickThreshold = false;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

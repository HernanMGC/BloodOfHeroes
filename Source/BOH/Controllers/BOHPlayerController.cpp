// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHPlayerController.h"

// UnrealEngine
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraFunctionLibrary.h"

// BOH
#include "BOH/Characters/BOHUnit.h"
#include "BOH/Component/Path/BOHPathLineActor.h"
#include "BOH/Component/Path/BOHPathPointActor.h"
#include "BOH/Component/Path/BOHUnitPathComponent.h"
#include "BOH/GameModes/BOHGameModeBase.h"
#include "BOH/Messages/BOHGameplayMessage.h"
#include "BOH/Pawns/BOHPlayerPawn.h"
#include "BOH/Tags/BOHGameplayTagCollection.h"
#include "BOH/Utils/BOHUtils.h"

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

DEFINE_LOG_CATEGORY(LogBOHPlayerController);

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

void ABOHPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABOHPlayerController, PlayerUnits);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::SpawnUnits(TArray<FTransform> UnitStartPointsTransforms)
{
	if (!HasAuthority()) { return; }

	UWorld* World = GetWorld();
	ABOHGameModeBase* GameMode = World ? World->GetAuthGameMode<ABOHGameModeBase>() : nullptr;
	if (!GameMode)
	{
		return;
	}

	// for (int32 i = 0; i < UnitStartPointsTransforms.Num(); ++i)
	int32 i = 0;
	{
		FActorSpawnParameters SpawnParameter;
		SpawnParameter.Instigator = this->GetPawn();
		SpawnParameter.Owner = this;
		SpawnParameter.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ABOHUnit* Unit = World->SpawnActorDeferred<ABOHUnit>(GameMode->GetDefaultUnitClass(), UnitStartPointsTransforms[i], this, this->GetPawn(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn, ESpawnActorScaleMethod::MultiplyWithRoot);
		if (Unit)
		{
		    // TODO: Change this for a data source. A data asset maybe? 
			PlayerUnits.AddUnique(Unit);
			FBOHUnitInfo UnitInfo = FBOHUnitInfo(
				static_cast<int32>(Unit->GetUniqueID()),
				static_cast<int32>(GetUniqueID()),
				EBOHUnitType::Enforcer,
				FMath::FRandRange(1.f, 3.f),
				FMath::FRandRange(20.f, 100.f),
				FMath::FRandRange(20.f, 100.f)
				);
			Unit->SetUnitInfo(UnitInfo);
			Unit->SetPlayer(this);
			Unit->FinishSpawning(UnitStartPointsTransforms[i]);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(this);
	OnUnitCommandMessageListenerHandle = GameplayMessageSubsystem.RegisterListener<
		FBOHSenderAuthorizedMessage>(UBOHGameplayTagCollection::Get().Tag_MessageChannel_UnitMoveCommand, this,
		                            &ThisClass::OnUnitMoveCommandReceived);


	OnUnitPathUpdateMessageListenerHandle = GameplayMessageSubsystem.RegisterListener<
		FBOHUnitPath>(UBOHGameplayTagCollection::Get().Tag_MessageChannel_UnitPathUpdate, this,
									&ThisClass::OnUnitPathUpdateReceived);
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

	UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(this);
	if (OnUnitCommandMessageListenerHandle.IsValid())
	{
		GameplayMessageSubsystem.UnregisterListener(OnUnitCommandMessageListenerHandle);
	}

	if (OnUnitPathUpdateMessageListenerHandle.IsValid())
	{
		GameplayMessageSubsystem.UnregisterListener(OnUnitPathUpdateMessageListenerHandle);
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
		BOH_LOG(LogBOHPlayerController, Error,
			"'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		    , *GetNameSafe(this));
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::OnSelectActorInputStarted()
{
	BOH_LOG(LogBOHPlayerController, Display, "OnInputStarted");
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
	const bool bHitSuccessful = World ? GetHitResultUnderCursor(EBOHCollisionChannel::ECC_PointNClick, true, Hit) : false;
	
	if (!bHitSuccessful)
	{
		return;
	}

	if (bIsInDoubleClickThreshold && LastHitActor == Hit.GetActor())
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

	LastHitActor = Hit.GetActor();
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
	if (const bool bHitSuccessful = GetHitResultUnderCursor(EBOHCollisionChannel::ECC_PointNClick, true, Hit))
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

void ABOHPlayerController::Server_SendUnitMoveCommand_Implementation(const TArray<FBOHUnitPath>& UnitsPath)
{
	UWorld* World = GetWorld();
	ABOHGameModeBase* GameMode = World ? World->GetAuthGameMode<ABOHGameModeBase>() : nullptr;
	if (!GameMode)
	{
		return;
	}

	GameMode->SubmitUnitsMoveCommand(this, UnitsPath);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::Client_TryUpdateUnitPath_Implementation(
	const FBOHUnitPath& UnitPathUpdateMessage)
{
	BOH_LOG_OBJECT(LogBOHPlayerController, Display, this, "DHER Client call");

	TObjectPtr<ABOHUnit> UpdatedUnit = UnitPathUpdateMessage.UnitPtr;
	if (!UpdatedUnit || !PlayerUnits.Contains(UpdatedUnit))
	{
		return;
	}

	UpdatedUnit->SetUnitPath(UnitPathUpdateMessage.UnitPath);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::OnUnitMoveCommandReceived(FGameplayTag GameplayTag,
                                                     const FBOHSenderAuthorizedMessage& UnitMoveCommandMessage)
{
	if (UnitMoveCommandMessage.Sender != this)
	{
		return;
	}
	
	TArray<FBOHUnitPath> UnitMoveCommands;
	for (TObjectPtr<ABOHUnit> Unit : PlayerUnits)
	{
		UBOHUnitPathComponent* PathComponent = Unit ? Unit->GetComponentByClass<UBOHUnitPathComponent>() : nullptr;
		if (!PathComponent) { continue; }
		
		FBOHUnitPath UnitMoveCommand;

		UnitMoveCommand.UnitPtr = Unit;
		UnitMoveCommand.UnitPath = PathComponent->GetUnitPath();

		UnitMoveCommands.Add(UnitMoveCommand);
	}

	Server_SendUnitMoveCommand(UnitMoveCommands);
}

void ABOHPlayerController::OnUnitPathUpdateReceived(FGameplayTag GameplayTag,
	const FBOHUnitPath& UnitPathUpdateMessage)
{
	if (!HasAuthority())
	{
		return;
	}
	
	TObjectPtr<ABOHUnit> UpdatedUnit = UnitPathUpdateMessage.UnitPtr;
	if (!UpdatedUnit || !PlayerUnits.Contains(UpdatedUnit))
	{
		return;
	}

	Client_TryUpdateUnitPath(UnitPathUpdateMessage);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerController::SetSelectedUnit(ABOHUnit* Unit)
{
	if (SelectedUnit && SelectedUnit != Unit)
	{
		SelectedUnit->SetIsUnitSelected(false);
	}

	SelectedUnit = Unit;

	OnUnitSelected.Broadcast(this, Unit);

	UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(this);
	FBOHSelectedUnitMessage SelectedUnitMessage;
	SelectedUnitMessage.Unit = Unit;
	SelectedUnitMessage.PlayerController = this;
	GameplayMessageSubsystem.BroadcastMessage(UBOHGameplayTagCollection::Get().Tag_MessageChannel_UnitSelected, SelectedUnitMessage);
	
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

	if (ABOHUnit* HitUnit = Cast<ABOHUnit>(Hit.GetActor()))
	{
		SetSelectedUnit(HitUnit);
		BOH_LOG(LogBOHPlayerController, Display, "Unit clicked and selected: %s", *SelectedUnit->GetUnitInfo().ToString());
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

	// No actor nor path actor clicked, and there is a valid SelectedUnit -> New point can be added to path.
	// TODO: Check valid course point?
	if (UBOHUnitPathComponent* PathComp = SelectedUnit ? SelectedUnit->GetComponentByClass<UBOHUnitPathComponent>() : nullptr)
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

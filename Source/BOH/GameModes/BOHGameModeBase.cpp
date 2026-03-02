// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHGameModeBase.h"

// UnrealEngine
#include "EngineUtils.h"
#include "Engine/PlayerStartPIE.h"
#include "GameFramework/PlayerStart.h"

// BOH
#include "BOH/AI/BOHAIController.h"
#include "BOH/Characters/BOHUnit.h"
#include "BOH/Controllers/BOHPlayerController.h"
#include "BOH/PlayerStarts/BOHPlayerStart.h"
#include "BOH/Utils/BOHUtils.h"

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

DEFINE_LOG_CATEGORY(LogBOHGameMode);

////////////////////////////////////////////////////////////////////////////////////
// FBOHUnitPathList
////////////////////////////////////////////////////////////////////////////////////

FBOHUnitPathList::FBOHUnitPathList() : UnitPaths(TArray<FBOHUnitPath>())
{
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

FBOHUnitPathList::FBOHUnitPathList(const TArray<FBOHUnitPath>& InUnitPaths) : UnitPaths(InUnitPaths)
{
}

////////////////////////////////////////////////////////////////////////////////////
// ABOHGameModeBase
////////////////////////////////////////////////////////////////////////////////////

void ABOHGameModeBase::SubmitUnitsMoveCommand(const ABOHPlayerController* Player, const TArray<FBOHUnitPath> UnitsPath)
{
	PlayerTurnsSubmitted.Add(Player, FBOHUnitPathList(UnitsPath));
	if (PlayerTurnsSubmitted.Num() < PlayersPerMatch)
	{
		return;
	}

	for (TPair<const ABOHPlayerController*, FBOHUnitPathList> PlayerTurn : PlayerTurnsSubmitted)
	{
		if (!PlayerTurn.Key) { continue; }
		
		for (FBOHUnitPath UnitPath : PlayerTurn.Value.UnitPaths)
		{
			BOH_LOG(LogBOHPlayerController, Display, "%s", *UnitPath.ToString());
			
			if (!PlayerTurn.Key->GetUnits().Contains(UnitPath.UnitPtr)) { continue; }

			if (UBOHUnitPathComponent* UnitPathComponent = UnitPath.UnitPtr->GetComponentByClass<UBOHUnitPathComponent>())
			{
				UnitPathComponent->SetUnitPath(UnitPath.UnitPath);
			}

			if (ABOHAIController* UnitAIController = Cast<ABOHAIController>(UnitPath.UnitPtr->GetController()))
			{
				UnitAIController->SendUnitOrder(FBOHUnitOrder(EUnitOrderType::MoveAlongPath, EUnitOrderSortingPolicy::AddToQueue, true, nullptr));
			}
		}
	}

	PlayerTurnsSubmitted.Empty();
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

AActor* ABOHGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	// DO NOT USE SUPER IMPLEMENTATION. WE WANT TO AVOID UNNECESSARY CODE AND WE WANT TO TOTALLY OVERRIDE THE BEHAVIOUR 
	// Choose a player start
	APlayerStart* FoundPlayerStart = nullptr;
	TArray<APlayerStart*> UnOccupiedStartPoints;
	UWorld* World = GetWorld();
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		APlayerStart* PlayerStart = *It;
		if (!PlayerStart) { continue; }
		
		BOH_LOG(LogBOHGameMode, Verbose, "[DHER] PlayerStart: %s", *PlayerStart->GetName());
		if (OccupiedPlayerStarts.Contains(PlayerStart))
		{
			continue;
		}
		
		if (PlayerStart->IsA<APlayerStartPIE>())
		{
			// Always prefer the first "Play from Here" PlayerStart, if we find one while in PIE mode
			FoundPlayerStart = PlayerStart;
			break;
		}

		UnOccupiedStartPoints.AddUnique(PlayerStart);
	}

	if (!FoundPlayerStart && !UnOccupiedStartPoints.IsEmpty())
	{
		FoundPlayerStart = UnOccupiedStartPoints[0];
	}
	
	check(FoundPlayerStart)

	OccupiedPlayerStarts.AddUnique(FoundPlayerStart);
	return FoundPlayerStart;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHGameModeBase::RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot)
{
	Super::RestartPlayerAtPlayerStart(NewPlayer, StartSpot);

	ABOHPlayerController* PlayerController = Cast<ABOHPlayerController>(NewPlayer);
	if (!PlayerController)
	{
		return;
	}

	ABOHPlayerStart* PlayerStart = Cast<ABOHPlayerStart>(StartSpot);
	if (!PlayerStart)
	{
		return;
	}

	PlayerController->SpawnUnits(PlayerStart->GetUnitSpawnPointsTransformWorld());
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

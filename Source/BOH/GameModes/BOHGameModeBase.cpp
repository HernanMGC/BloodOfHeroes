// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHGameModeBase.h"

// UnrealEngine
#include "EngineUtils.h"
#include "BOH/Controllers/BOHPlayerController.h"
#include "BOH/PlayerStarts/BOHPlayerStart.h"
#include "Engine/PlayerStartPIE.h"
#include "GameFramework/PlayerStart.h"

// BOH
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
		
		UE_LOG(LogTemp, Warning, TEXT("[DHER] PlayerStart: %s"), *PlayerStart->GetName());
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

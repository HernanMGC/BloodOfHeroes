// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHUnitLibFuncs.h"

// BOH
#include "BOH/Characters/BOHCharacter.h"
#include "BOH/GameModes/BOHGameModeBase.h"

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

float UBOHUnitLibFuncs::GetMaxDistanceForCharacter(const UObject* WorldContext, ABOHCharacter* Unit)
{
	UWorld* World = WorldContext && Unit ? WorldContext->GetWorld() : nullptr;
	ABOHGameModeBase* GameMode = World ? Cast<ABOHGameModeBase>(World->GetAuthGameMode()) : nullptr;
	if (!GameMode)
	{
		return 0.f;
	}

	return GameMode->GetTurnTime() * Unit->GetUnitInfo().Speed;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

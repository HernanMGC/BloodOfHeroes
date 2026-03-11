// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHUnitLibFuncs.h"

// BOH
#include "BOH/Characters/BOHUnit.h"
#include "BOH/GameStates/BOHGameState.h"

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

float UBOHUnitLibFuncs::GetMaxDistanceForUnitPerTurn(const UObject* WorldContext, ABOHUnit* Unit)
{
	UWorld* World = WorldContext && Unit ? WorldContext->GetWorld() : nullptr;
	ABOHGameState* GameState = World ? Cast<ABOHGameState>(World->GetGameState()) : nullptr;
	if (!GameState)
	{
		return 0.f;
	}

	return GameState->GetMaxTimePerTurn() * Unit->GetUnitInfo().Speed;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

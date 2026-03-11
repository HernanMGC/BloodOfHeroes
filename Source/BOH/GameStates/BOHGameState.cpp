// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHGameState.h"

// UnrealEngine
#include "Net/UnrealNetwork.h"

// BOH
#include "BOH/GameModes/BOHGameModeBase.h"
#include "BOH/Tags/BOHGameplayTagCollection.h"

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABOHGameState, MatchTotalTime);
	DOREPLIFETIME(ABOHGameState, MatchCurrentTime);
	DOREPLIFETIME(ABOHGameState, MaxTimePerTurn);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHGameState::OnRep_MatchTotalTime()
{
	UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(this);
	FBOHAuthorizedFloatMessage PlayersNameMessage;
	PlayersNameMessage.Sender = this;
	PlayersNameMessage.Number = GetMatchTotalTime();

	GameplayMessageSubsystem.BroadcastMessage(UBOHGameplayTagCollection::Get().Tag_MessageChannel_PlayersNameChanged, PlayersNameMessage);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHGameState::OnRep_MatchCurrentTime()
{
	UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(this);
	FBOHAuthorizedFloatMessage PlayersNameMessage;
	PlayersNameMessage.Sender = this;
	PlayersNameMessage.Number = GetMatchCurrentTime();

	GameplayMessageSubsystem.BroadcastMessage(UBOHGameplayTagCollection::Get().Tag_MessageChannel_MatchCurrentTimeChanged, PlayersNameMessage);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHGameState::SetMatchCurrentTime(float InMatchCurrentTime)
{
	MatchCurrentTime = InMatchCurrentTime;

	ENetMode NetMode = GetNetMode();
	if (NetMode == NM_Standalone || NetMode == NM_ListenServer)
	{
		OnRep_MatchCurrentTime();
	}
	
	ForceNetUpdate();
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHGameState::SetMatchTotalTime(float InMatchTotalTime)
{
	MatchTotalTime = InMatchTotalTime;

	ENetMode NetMode = GetNetMode();
	if (NetMode == NM_Standalone || NetMode == NM_ListenServer)
	{
		OnRep_MatchTotalTime();
	}
	
	ForceNetUpdate();
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHGameState::SetMaxTimePerTurn(float InMaxTimePerTurn)
{
	MaxTimePerTurn = InMaxTimePerTurn;
	ForceNetUpdate();
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHGameState::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	if (HasAuthority())
	{
		UWorld* World = GetWorld();
		if (ABOHGameModeBase* GameMode = World ? World->GetAuthGameMode<ABOHGameModeBase>(): nullptr)
		{
			SetMatchTotalTime(GameMode->GetMatchTotalTime());
			SetMatchCurrentTime(MatchTotalTime);
			SetMaxTimePerTurn(GameMode->GetMaxTimePerTurn());
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

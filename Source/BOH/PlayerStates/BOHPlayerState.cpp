// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHPlayerState.h"

// Unreal
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

// BOH
#include "BOH/Messages/BOHGameplayMessage.h"
#include "BOH/Tags/BOHGameplayTagCollection.h"
#include "BOH/Utils/BOHUtils.h"

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

ABOHPlayerState::ABOHPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	SetReplicates(true);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABOHPlayerState, TeamScore);
	DOREPLIFETIME(ABOHPlayerState, PlayerTurnState);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerState::SetTeamScore(int32 InTeamScore)
{
	TeamScore = InTeamScore;

	ENetMode NetMode = GetNetMode();
	if (NetMode == NM_Standalone || NetMode == NM_ListenServer)
	{
		OnRep_TeamScore();
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerState::SetTurnState(EBOHPlayerTurnState InTurnState)
{
	PlayerTurnState = InTurnState;

	ENetMode NetMode = GetNetMode();
	if (NetMode == NM_Standalone || NetMode == NM_ListenServer)
	{
		OnRep_PlayerTurnState();
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

	UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(this);
	FBOHAuthorizedTextMessage PlayersNameMessage;
	PlayersNameMessage.Sender = this;
	PlayersNameMessage.String = GetPlayerName();

	GameplayMessageSubsystem.BroadcastMessage(UBOHGameplayTagCollection::Get().Tag_MessageChannel_PlayersNameChanged, PlayersNameMessage);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerState::OnRep_TeamScore()
{
	UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(this);
	FBOHAuthorizedInt32Message PlayersScoreMessage;
	PlayersScoreMessage.Sender = this;
	PlayersScoreMessage.Number = TeamScore;

	GameplayMessageSubsystem.BroadcastMessage(UBOHGameplayTagCollection::Get().Tag_MessageChannel_PlayersScoreChanged, PlayersScoreMessage);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerState::OnRep_PlayerTurnState()
{
	UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(this);
	FBOHAuthorizedPlayerTurnStateMessage PlayersTurnStateMessage;
	PlayersTurnStateMessage.Sender = this;
	PlayersTurnStateMessage.TurnState = PlayerTurnState;

	GameplayMessageSubsystem.BroadcastMessage(UBOHGameplayTagCollection::Get().Tag_MessageChannel_PlayersTurnStateChanged, PlayersTurnStateMessage);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

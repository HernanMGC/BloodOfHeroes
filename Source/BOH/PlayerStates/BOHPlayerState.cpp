// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHPlayerState.h"

// Unreal
#include "BOH/Messages/BOHGameplayMessage.h"
#include "BOH/Tags/BOHGameplayTagCollection.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Net/UnrealNetwork.h"

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABOHPlayerState, PlayerScore);
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

void ABOHPlayerState::OnRep_PlayerScore()
{
	UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(this);
	FBOHAuthorizedInt32Message PlayersScoreMessage;
	PlayersScoreMessage.Sender = this;
	PlayersScoreMessage.Number = PlayerScore;

	GameplayMessageSubsystem.BroadcastMessage(UBOHGameplayTagCollection::Get().Tag_MessageChannel_PlayersScoreChanged, PlayersScoreMessage);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

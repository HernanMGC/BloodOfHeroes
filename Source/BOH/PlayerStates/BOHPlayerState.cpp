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

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ABOHPlayerState, TeamScore, SharedParams);
	DOREPLIFETIME_CONDITION_NOTIFY(ABOHPlayerState, TurnState, COND_None, REPNOTIFY_Always);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHPlayerState::SetTeamScore(int32 InTeamScore)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(ABOHPlayerState, TeamScore, this);
	TeamScore = InTeamScore;

	ENetMode NetMode = GetNetMode();
	if (NetMode == NM_Standalone || NetMode == NM_ListenServer)
	{
		OnRep_TeamScore();
	}
	
	ForceNetUpdate();
	BOH_LOG(LogTemp, Warning, "[DHER]");
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

	BOH_LOG(LogTemp, Warning, "[DHER]");
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
	BOH_LOG(LogTemp, Warning, "[DHER]");
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

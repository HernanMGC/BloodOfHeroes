// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHMatchStateViewModel.h"

// UnrealEngine
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Kismet/GameplayStatics.h"

// BOH
#include "BOH/Controllers/BOHPlayerController.h"
#include "BOH/GameStates/BOHGameState.h"
#include "BOH/Messages/BOHGameplayMessage.h"
#include "BOH/PlayerStates/BOHPlayerState.h"
#include "BOH/Tags/BOHGameplayTagCollection.h"
#include "BOH/Utils/BOHUtils.h"

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

DEFINE_LOG_CATEGORY(LogBOHMatchStateViewModel);

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHMatchStateViewModel::Init(FBOHViewModelInitParams& InitParams)
{
	Super::Init(InitParams);

	BOH_LOG(LogTemp, Warning, "[DHER]");

	OwningPlayerController = InitParams.OwningPlayerController;
	if (!OwningPlayerController)
	{
		return;
	}

	// Initial setup
	const int32 PlayersCount = UGameplayStatics::GetNumPlayerStates(OwningPlayerController);
	if (PlayersCount > BOHGameConstants::MaxPlayerPerMatch)
	{
		BOH_LOG(LogBOHMatchStateViewModel, Error, "Unexpected number of player. Max players per match is %d", BOHGameConstants::MaxPlayerPerMatch);
	}

	if (ABOHPlayerState* OwnPlayerState = Cast<ABOHPlayerState>(UGameplayStatics::GetPlayerState(OwningPlayerController, 0)))
	{
		UE_MVVM_SET_PROPERTY_VALUE(OwnPlayerName, OwnPlayerState->GetPlayerName());
		UE_MVVM_SET_PROPERTY_VALUE(OwnPlayerScore, OwnPlayerState->GetTeamScore());
	}

	if (ABOHPlayerState* OtherPlayerState = Cast<ABOHPlayerState>(UGameplayStatics::GetPlayerState(OwningPlayerController, 1)))
	{
		UE_MVVM_SET_PROPERTY_VALUE(OtherPlayerName, OtherPlayerState->GetPlayerName());
		UE_MVVM_SET_PROPERTY_VALUE(OtherPlayerScore, OtherPlayerState->GetTeamScore());
	}

	if (ABOHGameState* GameState = Cast<ABOHGameState>(UGameplayStatics::GetGameState(OwningPlayerController)))
	{
		UE_MVVM_SET_PROPERTY_VALUE(MatchTotalTime, GameState->GetMatchTotalTime());
		UE_MVVM_SET_PROPERTY_VALUE(MatchCurrentTime, GameState->GetMatchCurrentTime());
	}
		
	// Binding
	UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(OwningPlayerController);

	OnPlayersNameChangedMessageListenerHandle = GameplayMessageSubsystem.RegisterListener
	<FBOHAuthorizedTextMessage>(UBOHGameplayTagCollection::Get().Tag_MessageChannel_PlayersNameChanged, this,
								   &ThisClass::OnPlayersNameChanged);

	OnPlayersScoreChangedMessageListenerHandle = GameplayMessageSubsystem.RegisterListener
	<FBOHAuthorizedInt32Message>(UBOHGameplayTagCollection::Get().Tag_MessageChannel_PlayersScoreChanged, this,
								   &ThisClass::OnPlayersScoreChanged);

	OnMatchTotalTimeChangedMessageListenerHandle = GameplayMessageSubsystem.RegisterListener
	<FBOHAuthorizedFloatMessage>(UBOHGameplayTagCollection::Get().Tag_MessageChannel_MatchTotalTimeChanged, this,
								   &ThisClass::OnMatchTotalTimeChanged);

	OnMatchCurrentTimeChangedMessageListenerHandle = GameplayMessageSubsystem.RegisterListener
	<FBOHAuthorizedFloatMessage>(UBOHGameplayTagCollection::Get().Tag_MessageChannel_MatchCurrentTimeChanged, this,
								   &ThisClass::OnMatchCurrentTimeChanged);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHMatchStateViewModel::Deinit() const
{
	if (UWorld* World = OwningPlayerController ? OwningPlayerController->GetWorld() : nullptr)
	{
		UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(World);
		if (OnPlayersNameChangedMessageListenerHandle.IsValid())
		{
			GameplayMessageSubsystem.UnregisterListener(OnPlayersNameChangedMessageListenerHandle);
		}
		if (OnPlayersScoreChangedMessageListenerHandle.IsValid())
		{
			GameplayMessageSubsystem.UnregisterListener(OnPlayersScoreChangedMessageListenerHandle);
		}
		if (OnMatchTotalTimeChangedMessageListenerHandle.IsValid())
		{
			GameplayMessageSubsystem.UnregisterListener(OnMatchTotalTimeChangedMessageListenerHandle);
		}
		if (OnMatchCurrentTimeChangedMessageListenerHandle.IsValid())
		{
			GameplayMessageSubsystem.UnregisterListener(OnMatchCurrentTimeChangedMessageListenerHandle);
		}
	}
	BOH_LOG(LogTemp, Warning, "[DHER]");

	Super::Deinit();
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHMatchStateViewModel::OnPlayersNameChanged(FGameplayTag GameplayTag,
	const FBOHAuthorizedTextMessage& AuthorizedTextMessage)
{
	BOH_LOG(LogTemp, Warning, "[DHER]");

	if (AuthorizedTextMessage.Sender == UGameplayStatics::GetPlayerState(OwningPlayerController, 0))
	{
		UE_MVVM_SET_PROPERTY_VALUE(OwnPlayerName, AuthorizedTextMessage.String);
	}
	if (AuthorizedTextMessage.Sender == UGameplayStatics::GetPlayerState(OwningPlayerController, 1))
	{
		UE_MVVM_SET_PROPERTY_VALUE(OtherPlayerName, AuthorizedTextMessage.String);
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHMatchStateViewModel::OnPlayersScoreChanged(FGameplayTag GameplayTag,
	const FBOHAuthorizedInt32Message& AuthorizedInt32Message)
{
	BOH_LOG(LogTemp, Warning, "[DHER]");

	if (AuthorizedInt32Message.Sender.Get() == UGameplayStatics::GetPlayerState(OwningPlayerController, 0))
	{
		UE_MVVM_SET_PROPERTY_VALUE(OwnPlayerScore, AuthorizedInt32Message.Number);
	}
	if (AuthorizedInt32Message.Sender.Get() == UGameplayStatics::GetPlayerState(OwningPlayerController, 1))
	{
		UE_MVVM_SET_PROPERTY_VALUE(OtherPlayerScore, AuthorizedInt32Message.Number);
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHMatchStateViewModel::OnMatchTotalTimeChanged(FGameplayTag GameplayTag,
	const FBOHAuthorizedFloatMessage& AuthorizedFloatMessage)
{
	UE_MVVM_SET_PROPERTY_VALUE(MatchTotalTime, AuthorizedFloatMessage.Number);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHMatchStateViewModel::OnMatchCurrentTimeChanged(FGameplayTag GameplayTag,
	const FBOHAuthorizedFloatMessage& AuthorizedFloatMessage)
{
	UE_MVVM_SET_PROPERTY_VALUE(MatchCurrentTime, AuthorizedFloatMessage.Number);
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

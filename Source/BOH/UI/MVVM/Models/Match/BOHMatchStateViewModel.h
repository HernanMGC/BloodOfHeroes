// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// Unreal
#include "CoreMinimal.h"

// BOH
#include "BOH/Messages/BOHGameplayMessage.h"
#include "BOH/UI/MVVM/Models/BOHBaseViewModel.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "BOHMatchStateViewModel.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBOHMatchStateViewModel, Log, All);

/**
 * View model for match state visualization. It stores the players' names and their scores.
 * This viewmodel will be a global view model initialized by each player controller.
 */
UCLASS(BlueprintType)
class BOH_API UBOHMatchStateViewModel : public UBOHBaseViewModel
{
	GENERATED_BODY()
	public:
	// Overriden to: register to bind proper events as well as retrieve player and game state info for the first
	// initialization.
	virtual void Init(FBOHViewModelInitParams& InitParams) override;

	// Overriden to: unbind events.
	virtual void Deinit() const override;

protected:
	/**
	 * Reacts to players' name changed to update player name.
	 * @param GameplayTag 
	 * @param AuthorizedTextMessage 
	 */
	void OnPlayersNameChanged(FGameplayTag GameplayTag, const FBOHAuthorizedTextMessage& AuthorizedTextMessage);

	/**
	 * Reacts to players' score changed to update match score.
	 * @param GameplayTag 
	 * @param AuthorizedInt32Message 
	 */
	void OnPlayersScoreChanged(FGameplayTag GameplayTag, const FBOHAuthorizedInt32Message& AuthorizedInt32Message);

	/**
	 * 
	 * @param GameplayTag 
	 * @param AuthorizedFloatMessage 
	 */
	void OnMatchTotalTimeChanged(FGameplayTag GameplayTag, const FBOHAuthorizedFloatMessage& AuthorizedFloatMessage);

	/**
	 * 
	 * @param GameplayTag 
	 * @param AuthorizedFloatMessage 
	 */
	void OnMatchCurrentTimeChanged(FGameplayTag GameplayTag, const FBOHAuthorizedFloatMessage& AuthorizedFloatMessage);

	/**
	 * 
	 * @param GameplayTag 
	 * @param AuthorizedTurnStateMessage 
	 */
	void OnPlayersTurnStateChanged(FGameplayTag GameplayTag, const FBOHAuthorizedPlayerTurnStateMessage& AuthorizedTurnStateMessage);
	
protected:
	FGameplayMessageListenerHandle OnPlayersNameChangedMessageListenerHandle;
	FGameplayMessageListenerHandle OnPlayersScoreChangedMessageListenerHandle;
	FGameplayMessageListenerHandle OnMatchTotalTimeChangedMessageListenerHandle;
	FGameplayMessageListenerHandle OnMatchCurrentTimeChangedMessageListenerHandle;
	FGameplayMessageListenerHandle OnPlayersTurnStateChangedMessageListenerHandle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, FieldNotify)
	int32 OwnPlayerScore = INDEX_NONE;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, FieldNotify)
	int32 OtherPlayerScore = INDEX_NONE;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, FieldNotify)
	FString OwnPlayerName = TEXT("");
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, FieldNotify)
	FString OtherPlayerName = TEXT("");

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, FieldNotify)
	float MatchTotalTime = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, FieldNotify)
	float MatchCurrentTime = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, FieldNotify)
	EBOHPlayerTurnState OwnPlayerTurnState = EBOHPlayerTurnState::None;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, FieldNotify)
	EBOHPlayerTurnState OtherPlayerTurnState = EBOHPlayerTurnState::None;
};

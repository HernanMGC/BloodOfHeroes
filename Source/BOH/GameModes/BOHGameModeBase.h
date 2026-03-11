// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "BOH/Controllers/BOHPlayerController.h"
#include "GameFramework/GameMode.h"

// BOH
#include "BOHGameModeBase.generated.h"

class ABOHUnit;

DECLARE_LOG_CATEGORY_EXTERN(LogBOHGameMode, Log, All);

USTRUCT()
struct FBOHUnitPathList
{
	GENERATED_BODY()
public:
	UPROPERTY(Transient)
	TArray<FBOHUnitPath> UnitPaths;

public:
	FBOHUnitPathList();
	
	FBOHUnitPathList(const TArray<FBOHUnitPath>& InUnitPaths);
};

/**
 * Base game mode for Blood of Heroes.
 */
UCLASS(Abstract)
class BOH_API ABOHGameModeBase : public AGameMode
{
	GENERATED_BODY()

public:
	/**
	 * Returns turn time in seconds.
	 * @return 
	 */
	FORCEINLINE float GetMaxTimePerTurn() const { return MaxTimePerTurn; }

	/**
	 * Returns match total time in seconds.
	 * @return 
	 */
	FORCEINLINE float GetMatchTotalTime() const { return MatchTotalTime; }

	/**
	 * Returns default unit class.
	 * @return 
	 */
	FORCEINLINE TSubclassOf<ABOHUnit> GetDefaultUnitClass() const { return DefaultUnitClass; }

	/**
	 * Submit move command. Actual AI controller movement will only be executed when all clients have submitted their turns.
	 * @param Player
	 * @param UnitsPath 
	 */
	void SubmitUnitsMoveCommand(const ABOHPlayerController* Player, TArray<FBOHUnitPath> UnitsPath);

protected:
	// Overriden to: Prevent two player to start from the same player start.
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	// Overriden to: spawn units per controller.
	virtual void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;

	// Overriden to: set match score.
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = L"") override;

protected:
	// Turn time in seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BOH|Turn", meta = (Units = "s", ClampMin = 0.f))
	float MatchTotalTime = 150.f;

	// Turn time in seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BOH|Turn", meta = (Units = "s", ClampMin = 0.f))
	float MaxTimePerTurn = 3.f;

	// Players Per Match.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BOH|Players", meta = (ClampMin = 0))
	int32 PlayersPerMatch = 2;
	
	// Default unit class.
	UPROPERTY(EditAnywhere, Category = "BOH|Classes")
	TSubclassOf<ABOHUnit> DefaultUnitClass = nullptr;
	
	// List of occupied player starts as only one player per PlayerStart is allowed.
	UPROPERTY(Transient)
	TArray<APlayerStart*> OccupiedPlayerStarts;

	// List of submitted turns
	UPROPERTY(Transient)
	TMap<const ABOHPlayerController*, FBOHUnitPathList> PlayerTurnsSubmitted;
};

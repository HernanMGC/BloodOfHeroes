// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/IConsoleManager.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"

#include "BOHLogSettings.generated.h"

UCLASS(config=Game, defaultconfig)
class BOH_API UBOHLogSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:	
	UPROPERTY(config, EditAnywhere, Category="BOH|Log", meta=(ConsoleVariable="BOH.Log.LogNetMode"))
	bool LogNetMode = false;

	UPROPERTY(config, EditAnywhere, Category="BOH|Log", meta=(ConsoleVariable="BOH.Log.LogFunction"))
	bool LogFunction = true;

public:
	static bool GetLogNetMode();
	static bool GetLogFunction();
};

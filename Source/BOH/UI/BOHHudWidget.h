// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"

// BOH
#include "BOHHudWidget.generated.h"

/**
 * Base class for HUD Widget.
 */
UCLASS(Abstract)
class BOH_API UBOHHudWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
};

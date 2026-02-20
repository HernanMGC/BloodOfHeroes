// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

// BOH
#include "BOHHUD.generated.h"

//// ForwardDeclarations
// BOH
class UBOHHudWidget;

/**
 * 
 */
UCLASS()
class BOH_API ABOHHUD : public AHUD
{
	GENERATED_BODY()

public:
	// HUD Widget class.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BOH|UI")
	TSubclassOf<UBOHHudWidget> HUDWidgetClass = nullptr;

protected:
	// Overriden to: add HUD widget to viewport.
	virtual void BeginPlay() override;

protected:
	// HUD Widget reference.
	TObjectPtr<UBOHHudWidget> HUDWidget = nullptr; 
};

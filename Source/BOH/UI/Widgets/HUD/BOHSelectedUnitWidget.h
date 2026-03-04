// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"
#include "CommonUserWidget.h"

// BOH
#include "BOHSelectedUnitWidget.generated.h"

//// ForwardDeclarations
// BOH
class UBOHVMPlayerUnits;

/**
 * Base class for selected unit widget.
 */
UCLASS(Abstract)
class BOH_API UBOHSelectedUnitWidget : public UCommonUserWidget
{
	GENERATED_BODY()

protected:
	// Overriden to: create and initialize ViewModel-
	virtual void NativeConstruct() override;

	// Overriden to: destroy ViewModel.
	virtual void NativeDestruct() override;
	
protected:
	UPROPERTY(Transient, BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<UBOHVMPlayerUnits> PlayerViewModel = nullptr;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "BOHBaseViewModel.generated.h"

/**
 * Base class view model init params.
 */
USTRUCT()
struct FBOHViewModelInitParams
{
	GENERATED_BODY()
};

/**
 * A base class for BOH's ViewModels
 */
UCLASS(Abstract)
class BOH_API UBOHBaseViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/**
	 * Initializes view model.
	 * @param InitParams 
	 */
	virtual void Init(FBOHViewModelInitParams& InitParams) {};

protected:
	/**
	 * Deinitializes view model.
	 */
	virtual void Deinit() const {};

	/**
	 * Calls for deinitialization.
	 */
	virtual void BeginDestroy() override;
};

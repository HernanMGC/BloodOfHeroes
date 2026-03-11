// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Types/MVVMViewModelCollection.h"
#include "BOHBaseViewModel.generated.h"

//// ForwardDeclarations
// BOH
class ABOHPlayerController;

/**
 * Base class view model init params.
 */
USTRUCT()
struct FViewModelInitParams
{
	GENERATED_BODY()
};

namespace ViewModelConsts
{
	template<typename ViewModelType, typename ViewModelParamsType>
	static TObjectPtr<ViewModelType> InitGlobalViewModel(TObjectPtr<UMVVMViewModelCollectionObject> GlobalViewModelCollection,
														 ViewModelParamsType						ViewModelInitParams,
														 const UObject* WorldContextObject)
	{
		FMVVMViewModelContext ViewModelContext;
		ViewModelContext.ContextClass = ViewModelType::StaticClass();
		ViewModelContext.ContextName = ViewModelContext.ContextClass->GetFName();

		if (!ViewModelContext.IsValid())
		{
			return nullptr;
		}

		GlobalViewModelCollection->RemoveViewModel(ViewModelContext);
		
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		check(World);
		TObjectPtr<ViewModelType> ViewModel = NewObject<ViewModelType>(World);
		GlobalViewModelCollection->AddViewModelInstance(ViewModelContext, ViewModel);

		check(ViewModel);
		ViewModel->Init(ViewModelInitParams);
		return ViewModel;
	}

	template<typename ViewModelType>
	static TObjectPtr<ViewModelType> FindGlobalViewModel(TObjectPtr<UMVVMViewModelCollectionObject> GlobalViewModelCollection)
	{
		FMVVMViewModelContext ViewModelContext;
		ViewModelContext.ContextClass = ViewModelType::StaticClass();
		ViewModelContext.ContextName = ViewModelContext.ContextClass->GetFName();

		if (!ViewModelContext.IsValid())
		{
			return nullptr;
		}

		return Cast<ViewModelType>(GlobalViewModelCollection->FindViewModelInstance(ViewModelContext));
	}
}

/**
 * Base class view model init params.
 */
USTRUCT()
struct FBOHViewModelInitParams
{
	GENERATED_BODY()
public:
	FBOHViewModelInitParams();
	
	FBOHViewModelInitParams(ABOHPlayerController* InOwningPlayerController);

public:
	// View owning player controller.
	UPROPERTY(Transient)
	TObjectPtr<ABOHPlayerController> OwningPlayerController = nullptr;
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
	
protected:
	// Owning player controller. 
	UPROPERTY(Transient)
	TObjectPtr<ABOHPlayerController> OwningPlayerController = nullptr;
};

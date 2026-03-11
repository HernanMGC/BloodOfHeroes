// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHHUD.h"

// UnrealEngine
#include "Blueprint/UserWidget.h"
#include "MVVMGameSubsystem.h"

// BOH
#include "BOH/Controllers/BOHPlayerController.h"
#include "BOH/UI/MVVM/Models/BOHBaseViewModel.h"
#include "BOH/UI/MVVM/Models/Match/BOHMatchStateViewModel.h"
#include "BOH/UI/Widgets/HUD/BOHHudWidget.h"

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void ABOHHUD::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance*		GameInstance = GetGameInstance();
	UMVVMGameSubsystem* ViewModelGameSubsystem = GameInstance ? GameInstance->GetSubsystem<UMVVMGameSubsystem>() : nullptr;
	check(ViewModelGameSubsystem);

	UMVVMViewModelCollectionObject* GlobalViewModelCollection = ViewModelGameSubsystem->GetViewModelCollection();
	check(GlobalViewModelCollection);
	
	FBOHViewModelInitParams MatchStateViewModelInitParams;
	MatchStateViewModelInitParams.OwningPlayerController = Cast<ABOHPlayerController>(GetOwningPlayerController());
	MatchStateViewModel = ViewModelConsts::InitGlobalViewModel<
		UBOHMatchStateViewModel, FBOHViewModelInitParams>(
		GlobalViewModelCollection, MatchStateViewModelInitParams, this);

	
	HUDWidget = CreateWidget<UBOHHudWidget>(GetOwningPlayerController(), HUDWidgetClass, TEXT("HUD"));
	if (!HUDWidget)
	{
		return;
	}
	HUDWidget->AddToViewport(0);
	
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

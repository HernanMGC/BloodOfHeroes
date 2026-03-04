// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
// Class
#include "BOHSelectedUnitWidget.h"

// BOH
#include "BOH/Controllers/BOHPlayerController.h"
#include "BOH/UI/MVVM/Models/Units/BOHVMPlayerUnits.h"
#include "View/MVVMView.h"

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHSelectedUnitWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UMVVMView* View = Cast<UMVVMView>(GetExtension(UMVVMView::StaticClass()));
	FName PlayerViewModelClassName(UBOHVMPlayerUnits::StaticClass()->GetName()); 
	TScriptInterface<INotifyFieldValueChanged> ViewModelIface = View ? View->GetViewModel(PlayerViewModelClassName) : nullptr;
	PlayerViewModel = ViewModelIface ? Cast<UBOHVMPlayerUnits>(ViewModelIface.GetObject()) : nullptr;

	if (PlayerViewModel)
	{
		FBOHViewModelInitParams VMInitParams;
		VMInitParams.OwningPlayerController = Cast<ABOHPlayerController>(GetOwningPlayer());
		PlayerViewModel->Init(VMInitParams);
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

void UBOHSelectedUnitWidget::NativeDestruct()
{
	PlayerViewModel = nullptr;
	Super::NativeDestruct();
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

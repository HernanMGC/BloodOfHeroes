// Fill out your copyright notice in the Description page of Project Settings.

//// Includes
#include "BOHUnitOrders.h"

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

FBOHUnitOrder::FBOHUnitOrder() : OrderType(EUnitOrderType::None), OrderState(EUnitOrderState::None), bCanBeInterrupted(false),
                           TargetActor(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

FBOHUnitOrder::FBOHUnitOrder(EUnitOrderType InOrderType, EUnitOrderSortingPolicy InUnitOrderSortingPolicy,
                       bool bInCanBeInterrupted, AActor* InTargetActor) : OrderType(InOrderType),
                                                                           OrderState(EUnitOrderState::Queued),
                                                                           OrderSortingPolicy(InUnitOrderSortingPolicy),
                                                                           bCanBeInterrupted(bInCanBeInterrupted),
                                                                           TargetActor(InTargetActor)
{
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

bool FBOHUnitOrder::operator==(const FBOHUnitOrder& Other) const
{
	return OrderType == Other.OrderType
		&& OrderSortingPolicy == Other.OrderSortingPolicy
		&& OrderState == Other.OrderState
		&& bCanBeInterrupted == Other.bCanBeInterrupted
		&& TargetActor == Other.TargetActor;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

bool FBOHUnitOrder::IsValid() const
{
	return
		OrderType != EUnitOrderType::None
		&& OrderType != EUnitOrderType::MAX
		&& OrderState != EUnitOrderState::None
		&& OrderState != EUnitOrderState::MAX
		&& OrderSortingPolicy != EUnitOrderSortingPolicy::None
		&& OrderSortingPolicy != EUnitOrderSortingPolicy::MAX;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

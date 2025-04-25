// Fill out your copyright notice in the Description page of Project Settings.

#include "BOH/Characters/BOHPawnData.h"
#include "GameFramework/Pawn.h"

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

UBOHPawnData::UBOHPawnData(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PawnClass = nullptr;
	InputConfig = nullptr;
}
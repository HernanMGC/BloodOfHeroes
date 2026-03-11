// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//// Includes
// UnrealEngine
#include "CoreMinimal.h"

namespace BOHGameConstants
{
	inline constexpr int32 MaxPlayerPerMatch = 2;
}

namespace BOHUnitConstants
{
	inline constexpr float CentimetersToMeters = 0.01f;
	inline constexpr float MetersToCentimeters = 100.0f;
}

namespace EBOHCollisionChannel
{
	static ECollisionChannel ECC_PointNClick = ECC_GameTraceChannel1;
}

inline FString GetGWorldNetMode()
{
	if (!GWorld)
	{
		return "NO GWorld";
	}

	int32 PIEID = static_cast<int32>(GPlayInEditorID);
	switch (GWorld->GetNetMode())
	{
	case NM_Standalone:
		return FString::Printf(TEXT("NM_Standalone[%d]"), PIEID);
		break;
	case NM_DedicatedServer:
		return FString::Printf(TEXT("NM_DedicatedServer[%d]"), PIEID);
		break;
	case NM_ListenServer:
		return FString::Printf(TEXT("NM_ListenServer[%d]"), PIEID);
		break;
	case NM_Client:
		return FString::Printf(TEXT("NM_Client[%d]"), PIEID);
		break;
	case NM_MAX:
		return FString::Printf(TEXT("NM_MAX[%d]"), PIEID);
		break;
	default: ;
		return "NONE";
	}
};

inline FString GetNetRole(UObject* Context)
{
	FString Str = FString("NetRole::Undetermined");

	
	if (UActorComponent* ActorComponent = Cast<UActorComponent>(Context))
	{
		AActor* OwnerActor = ActorComponent->GetOwner();
		UNetConnection* NetConnection = OwnerActor->GetNetConnection();
		FString NetConnectionStr = NetConnection ? NetConnection->GetName() : FString("NoNetConnection");
		Str = FString::Printf(TEXT("OwnerRole::%s|Owner::%s|OwningConn::%s"), *UEnum::GetValueAsString(ActorComponent->GetOwnerRole()), *OwnerActor->GetName(), *NetConnectionStr);
	}
	else if (AActor* Actor = Cast<AActor>(Context))
	{
		UNetConnection* NetConnection = Actor->GetNetConnection();
		FString NetConnectionStr = NetConnection ? NetConnection->GetName() : FString("NoNetConnection");
		AActor* ActorOwner = Actor->GetOwner();
		FString ActorOwnerStr = ActorOwner ? ActorOwner->GetName() : FString("NoOwner");
		Str = FString::Printf(TEXT("LocalRole::%s|Owner::%s|OwningConn::%s"), *UEnum::GetValueAsString(Actor->GetLocalRole()), *ActorOwnerStr, *NetConnectionStr);
	}
	
	return Str;
}

//Current Class Name + Function Name where this is called!
#define TRACE_STR_CUR_CLASS_FUNC (FString(__FUNCTION__))
//Current Class where this is called!
#define TRACE_STR_CUR_CLASS (FString(__FUNCTION__).Left(FString(__FUNCTION__).Find(TEXT(":"))) )
//Current Function Name where this is called!
#define TRACE_STR_CUR_FUNC (FString(__FUNCTION__).Right(FString(__FUNCTION__).Len() - FString(__FUNCTION__).Find(TEXT("::")) - 2 ))
//Current Line Number in the code where this is called!
#define TRACE_STR_CUR_LINE  (FString::FromInt(__LINE__))
//Current Class and Line Number where this is called!
#define TRACE_STR_CUR_CLASS_LINE (TRACE_STR_CUR_CLASS + "(" + TRACE_STR_CUR_LINE + ")")
//Current Class Name + Function Name and Line Number where this is called!
#define TRACE_STR_CUR_CLASS_FUNC_LINE (TRACE_STR_CUR_CLASS_FUNC + "(" + TRACE_STR_CUR_LINE + ")")
//Current Function Signature where this is called!
#define TRACE_STR_CUR_FUNCSIG (FString(__FUNCSIG__))

//Screen Message
#define TRACE_SCREENMSG(OutputMessage) (GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, *(TRACE_STR_CUR_CLASS_FUNC_LINE + ": " + OutputMessage)) )
#define TRACE_SCREENMSG_PRINTF(FormatString , ...) (GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, *(TRACE_STR_CUR_CLASS_FUNC_LINE + ": " + (FString::Printf(TEXT(FormatString), ##__VA_ARGS__ )))) )

//UE LOG!
#define BOH_LOG_OBJECT(LogCat, Verbosity, Object, FormatString, ...) UE_LOG(LogCat,Verbosity,TEXT("[%s][%s] %s: %s"), *GetGWorldNetMode(), *GetNetRole(Object), *TRACE_STR_CUR_CLASS_FUNC_LINE, *FString::Printf(TEXT(FormatString), ##__VA_ARGS__ ) )
#define BOH_LOG(LogCat, Verbosity, FormatString, ...) UE_LOG(LogCat,Verbosity,TEXT("[%s] %s: %s"), *GetGWorldNetMode(), *TRACE_STR_CUR_CLASS_FUNC_LINE, *FString::Printf(TEXT(FormatString), ##__VA_ARGS__ ) )
#define TRACE_LOG(LogCategory, OutputMessage) UE_LOG(LogCategory,Log,TEXT("%s: %s"), *TRACE_STR_CUR_CLASS_FUNC_LINE, *FString(OutputMessage))
#define TRACE_LOG_PRINTF(LogCat, FormatString , ...) UE_LOG(LogCat,Log,TEXT("%s: %s"), *TRACE_STR_CUR_CLASS_FUNC_LINE, *FString::Printf(TEXT(FormatString), ##__VA_ARGS__ ) )
#define TRACE_WARNING(LogCategory, OutputMessage) UE_LOG(LogCategory,Warning,TEXT("%s: %s"), *TRACE_STR_CUR_CLASS_FUNC_LINE, *FString(OutputMessage))
#define TRACE_WARNING_PRINTF(LogCategory, FormatString , ...) UE_LOG(LogCategory,Warning,TEXT("%s: %s"), *TRACE_STR_CUR_CLASS_FUNC_LINE, *FString::Printf(TEXT(FormatString), ##__VA_ARGS__ ) )
#define TRACE_ERROR(LogCategory, OutputMessage) UE_LOG(LogCategory,Error,TEXT("%s: %s"), *TRACE_STR_CUR_CLASS_FUNC_LINE, *FString(OutputMessage))
#define TRACE_ERROR_PRINTF(LogCategory, FormatString , ...) UE_LOG(LogCategory,Error,TEXT("%s: %s"), *TRACE_STR_CUR_CLASS_FUNC_LINE, *FString::Printf(TEXT(FormatString), ##__VA_ARGS__ ) )
#define TRACE_FATAL(LogCategory, OutputMessage) UE_LOG(LogCategory,Fatal,TEXT("%s: %s"), *TRACE_STR_CUR_CLASS_FUNC_LINE, *FString(OutputMessage))
#pragma once

//// Includes
// UnrealEngine
#include "Logging/LogMacros.h"

// BOH
#include "BOHLogSettings.h"

// returns NetMode string for the given context object. If no context object is given, it will try to get the current world context from GEngine.
BOH_API FString GetNetModeString(const UObject* ContextObject = nullptr);

#define LOG_NETMODE_ENABLED UBOHLogSettings::GetLogNetMode()
#define LOG_FUNCTION_ENABLED UBOHLogSettings::GetLogFunction()

// __func__ is mandated by the C++11 standard so it's guaranteed to work
#define LOG_FUNCTION *("[" + FString(__func__) + "]")
#define LOG_CLASS_METHOD *("[" + FString(StaticClass()->GetPrefixCPP()) + FString(StaticClass()->GetName()) + "::" + FString(__func__) + "]")
#define LOG_STRUCT_METHOD *("[" + FString(StaticStruct()->GetPrefixCPP()) + FString(StaticStruct()->GetName()) + "::" + FString(__func__) + "]")

// __FILE__ and __LINE__ are used in ensure() and check() macros so they're guaranteed to work
#define LOG_FILE_LINE *("[" + FString(__FILE__) + "(" + FString::FromInt(__LINE__) + ")]")

// Logs NetMode for the given context object. If no context object is given, it will try to get the current world context from GEngine.
#define LOG_NETMODE *(GetNetModeString())

#define PRINT_TABLE_ROW(__src__, __dst__, __len__) \
{ \
	const FString DataToPrint = __src__; \
	__dst__ += DataToPrint; \
	for (int32 i = DataToPrint.Len(); i < __len__; i++) __dst__ += FString::Printf(TEXT(" ")); \
}

// Log macro that also prints NetMode and Class::Method. NetMode might be empty if no world context is found.
#define BOH_LOG(CategoryName, Verbosity, Format, ...) \
{ \
	if(LOG_NETMODE_ENABLED && LOG_FUNCTION_ENABLED) \
	{ \
		UE_LOG(CategoryName, Verbosity, TEXT("%s %s - " Format), LOG_NETMODE, LOG_FUNCTION, ##__VA_ARGS__) \
	} \
	else if (LOG_NETMODE_ENABLED && !LOG_FUNCTION_ENABLED) \
	{ \
		UE_LOG(CategoryName, Verbosity, TEXT("%s - " Format), LOG_NETMODE, ##__VA_ARGS__) \
	} \
	else if (!LOG_NETMODE_ENABLED && LOG_FUNCTION_ENABLED) \
	{ \
		UE_LOG(CategoryName, Verbosity, TEXT("%s - " Format), LOG_FUNCTION, ##__VA_ARGS__) \
	} \
	else \
	{ \
		UE_LOG(CategoryName, Verbosity, Format, ##__VA_ARGS__) \
	} \
}

//
#define BOH_LOG_STRUCT(CategoryName, Verbosity, Format, ...) \
{ \
	if(LOG_FUNCTION_ENABLED && LOG_NETMODE_ENABLED) \
	{ \
		UE_LOG(CategoryName, Verbosity, TEXT("%s %s - " Format), LOG_NETMODE, LOG_STRUCT_METHOD, ##__VA_ARGS__) \
	} \
	else if (!LOG_FUNCTION_ENABLED && LOG_NETMODE_ENABLED) \
	{ \
		UE_LOG(CategoryName, Verbosity, TEXT("%s - " Format), LOG_NETMODE, ##__VA_ARGS__) \
	} \
	else if (LOG_FUNCTION_ENABLED && !LOG_NETMODE_ENABLED) \
	{ \
		UE_LOG(CategoryName, Verbosity, TEXT("%s - " Format), LOG_STRUCT_METHOD, ##__VA_ARGS__) \
	} \
	else \
	{ \
		UE_LOG(CategoryName, Verbosity, Format, ##__VA_ARGS__) \
	} \
}

//
#define BOH_LOG_CLASS(CategoryName, Verbosity, Format, ...) \
{ \
	if(LOG_NETMODE_ENABLED && LOG_FUNCTION_ENABLED) \
	{ \
		UE_LOG(CategoryName, Verbosity, TEXT("%s %s - " Format), LOG_NETMODE, LOG_CLASS_METHOD, ##__VA_ARGS__) \
	} \
	else if (LOG_NETMODE_ENABLED && !LOG_FUNCTION_ENABLED) \
	{ \
		UE_LOG(CategoryName, Verbosity, TEXT("%s - " Format), LOG_NETMODE, ##__VA_ARGS__) \
	} \
	else if (!LOG_NETMODE_ENABLED && LOG_FUNCTION_ENABLED) \
	{ \
		UE_LOG(CategoryName, Verbosity, TEXT("%s - " Format), LOG_CLASS_METHOD, ##__VA_ARGS__) \
	} \
	else \
	{ \
		UE_LOG(CategoryName, Verbosity, Format, ##__VA_ARGS__) \
	} \
}

#define BOH_CLOG_CLASS(Condition, CategoryName, Verbosity, Format, ...) \
{ \
	if (Condition) \
	{ \
		BOH_LOG_CLASS(CategoryName, Verbosity, Format, ##__VA_ARGS__) \
	} \
}

#define BOH_CLOG_STRUCT(Condition, CategoryName, Verbosity, Format, ...) \
{ \
	if (Condition) \
	{ \
		BOH_LOG_STRUCT(CategoryName, Verbosity, Format, ##__VA_ARGS__) \
	} \
}

#define BOH_CLOG(Condition, CategoryName, Verbosity, Format, ...) \
{ \
	if (Condition) \
	{ \
		BOH_LOG(CategoryName, Verbosity, Format, ##__VA_ARGS__) \
	} \
}

#define BOH_LOG_CLASS_ONCE(Verbosity, Format, ...) \
{ \
	static bool bLogged = false; \
	BOH_CLOG_CLASS(!bLogged, Verbosity, Format, ##__VA_ARGS__); \
	bLogged = true; \
}

#define BOH_LOG_STRUCT_ONCE(Verbosity, Format, ...) \
{ \
	static bool bLogged = false; \
	BOH_CLOG_STRUCT(!bLogged, Verbosity, Format, ##__VA_ARGS__); \
	bLogged = true; \
}

#define BOH_LOG_ONCE(Verbosity, Format, ...) \
{ \
	static bool bLogged = false; \
	BOH_CLOG(!bLogged, Verbosity, Format, ##__VA_ARGS__); \
	bLogged = true; \
}
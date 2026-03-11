// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BOH : ModuleRules
{
	public BOH(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			// CORE
			"Core",
			"NetCore",
			"CoreOnline",
			"CoreUObject",
			"Engine",
			
			// Input
			"InputCore",
			"EnhancedInput",
			
			// UI
			"CommonUI",
			"UMG",
			
			// AI
			"AIModule",
			
			// GAS
			"GameplayAbilities",
			"GameplayTasks",
			"GameplayTags",
			
			// GMS
			"GameplayMessageRuntime"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			// FX
			"Niagara",
			
			// UI
			"CommonUI",
			"ModelViewViewModel"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
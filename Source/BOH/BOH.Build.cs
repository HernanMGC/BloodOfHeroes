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
			"CoreUObject",
			"Engine",
			
			// Settings
			"DeveloperSettings",

			// Input
			"InputCore",
			"EnhancedInput",
			
			// UI
			"CommonUI",
			"UMG",
			
			// AI
			"AIModule",
			
			// Modular
			"ModularGameplay",
			"ModularGameplayActors",
			
			// GAS
			"GameplayTags",
			"GameplayAbilities",
			"GameplayTasks"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			// FX
			"Niagara",
			
			// UI
			"CommonUI",
			
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
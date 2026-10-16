// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Rougelike : ModuleRules
{
	public Rougelike(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Rougelike",
			"Rougelike/Variant_Platforming",
			"Rougelike/Variant_Platforming/Animation",
			"Rougelike/Variant_Combat",
			"Rougelike/Variant_Combat/AI",
			"Rougelike/Variant_Combat/Animation",
			"Rougelike/Variant_Combat/Gameplay",
			"Rougelike/Variant_Combat/Interfaces",
			"Rougelike/Variant_Combat/UI",
			"Rougelike/Variant_SideScrolling",
			"Rougelike/Variant_SideScrolling/AI",
			"Rougelike/Variant_SideScrolling/Gameplay",
			"Rougelike/Variant_SideScrolling/Interfaces",
			"Rougelike/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

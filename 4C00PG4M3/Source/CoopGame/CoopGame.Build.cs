// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CoopGame : ModuleRules
{
	public CoopGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput" 
		});
		
		PrivateDependencyModuleNames.AddRange(new string[] 
		{ 
			"OnlineSubsystem", 
			"OnlineSubsystemUtils",
			"OnlineSubsystemEOS",
			"Slate", 
			"SlateCore", 
			"OnlineServicesInterface"
		});
	}
}
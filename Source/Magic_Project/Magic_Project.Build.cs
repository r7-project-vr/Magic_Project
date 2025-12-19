// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Magic_Project : ModuleRules
{
	public Magic_Project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Niagara", "HeadMountedDisplay", "XRBase", "VRM4U", "BleGoodies" });

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			//PublicDependencyModuleNames.AddRange(new string[] { "ASerialCom" });
		}
		else if (Target.Platform == UnrealTargetPlatform.Android)
		{
			PublicDependencyModuleNames.AddRange(new string[] {  "AndroidPermission" });
		}

			PrivateDependencyModuleNames.AddRange(new string[] { });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

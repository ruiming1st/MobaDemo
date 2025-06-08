// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MobyGame : ModuleRules
{
	public MobyGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "HeadMountedDisplay",
            "NavigationSystem",
            "AIModule",
            "SimpleScreenMove",
            "LandScape",
            "SimpleThread",
            "SlateCore",
            "SimpleDrawText"
        });

        if (Target.bWithServerCode == true)
        {
            PublicDependencyModuleNames.AddRange(new string[] { });
        }
        else
        {
            PublicDependencyModuleNames.AddRange(new string[] {"ImageWrapper"});
        }
      
    }
}

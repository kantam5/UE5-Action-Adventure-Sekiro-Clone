// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ActionAdventure : ModuleRules
{
	public ActionAdventure(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "MotionWarping", "NetCore", "Niagara", "AIModule", "UMG", "NavigationSystem", "CableComponent" });

        PublicIncludePaths.Add("ActionAdventure/");

        PrivateDependencyModuleNames.AddRange(new string[]{
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",
            "Slate",
            "SlateCore"
        });
    }
}

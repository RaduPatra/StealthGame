// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StealthGame : ModuleRules
{
	public StealthGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
			{ "Core", "CoreUObject", "Engine", "InputCore", "AIModule", "HeadMountedDisplay", "GameplayTasks", "GameplayTags", "MotionWarping", "NavigationSystem"});
	}
}
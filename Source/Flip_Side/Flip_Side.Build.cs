// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Flip_Side : ModuleRules
{
	public Flip_Side(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"SQLiteCore", 
			"SQLiteSupport", 
			"Niagara",
			"UMG"
			 });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PublicIncludePaths.AddRange(new string[] {
			"Flip_Side",
			"Flip_Side/DataTypes",
			"Flip_Side/Player",
			"Flip_Side/UI",
			"Flip_Side/Subsystem",
			"Flip_Side/Subsystem/BattleLevel",
			"Flip_Side/Subsystem/ShopLevel",
			"Flip_Side/Objects",
			"Flip_Side/Interface",
			"Flip_Side/UI/ShopCard",
			"Flip_Side/UI/ShopCoinCreate",
			"Flip_Side/UI/ShopCoinManage",
			"Flip_Side/UI/ShopItem",
			"Flip_Side/UI/ShopUnlockCard",
			"Flip_Side/UI/ShopUnlockWeapon",
			"Flip_Side/BossPattern",
			"Flip_Side/Actors",
			"Flip_Side/ShopTutorial"
		});


		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

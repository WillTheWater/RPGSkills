// WillTheWater

using UnrealBuildTool;
using System.Collections.Generic;

public class RPGSkillsTarget : TargetRules
{
	public RPGSkillsTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "RPGSkills" } );
	}
}

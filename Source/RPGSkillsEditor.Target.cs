// WillTheWater

using UnrealBuildTool;
using System.Collections.Generic;

public class RPGSkillsEditorTarget : TargetRules
{
	public RPGSkillsEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "RPGSkills" } );
	}
}

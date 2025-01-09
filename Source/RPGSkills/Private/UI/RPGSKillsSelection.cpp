// WillTheWater

#include "UI/RPGSKillsSelection.h"

void URPGSKillsSelection::SelectSkill(ESkills Skill)
{
	if (PlayerReference)
	{
		PlayerReference->ActiveSkill = Skill;
	}
}

// WillTheWater

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Characters/RPGSkillsBaseCharacter.h"
#include "RPGSKillsSelection.generated.h"

class ARPGSkillsBaseCharacter;

UCLASS()
class RPGSKILLS_API URPGSKillsSelection : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ARPGSkillsBaseCharacter* PlayerReference;

	UFUNCTION(BlueprintCallable)
	void SelectSkill(ESkills Skill);
	
};

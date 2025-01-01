// WillTheWater

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Characters/RPGSkillsBaseCharacter.h"
#include "RPGOverlayUI.generated.h"

class ARPGSkillsBaseCharacter;

UCLASS()
class RPGSKILLS_API URPGOverlayUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ARPGSkillsBaseCharacter* PlayerReference;
	
};

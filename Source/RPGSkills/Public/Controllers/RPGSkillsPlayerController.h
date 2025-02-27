// WillTheWater

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RPGSkillsPlayerController.generated.h"

UCLASS()
class RPGSKILLS_API ARPGSkillsPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
};

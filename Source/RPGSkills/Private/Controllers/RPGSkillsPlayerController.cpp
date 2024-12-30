// WillTheWater


#include "Controllers/RPGSkillsPlayerController.h"

void ARPGSkillsPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameOnly());
}

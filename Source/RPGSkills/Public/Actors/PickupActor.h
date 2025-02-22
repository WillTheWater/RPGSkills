// WillTheWater

#pragma once

#include "CoreMinimal.h"
#include "Actors/InteractBase.h"
#include "PickupActor.generated.h"

class UUserWidget;

UCLASS()
class RPGSKILLS_API APickupActor : public AInteractBase
{
	GENERATED_BODY()

	public:
	APickupActor();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> PickupWidgetClass;

	UUserWidget* PickUI;
	FTimerHandle DelayResetTimer;

	virtual void PlayerEnderedCPP() override;
	virtual void PlayerLeftCPP() override;
	virtual void ToggleIntereaction(ARPGSkillsBaseCharacter* PlayerReference) override;
	virtual void NextActionInteractActor() override;

	void RemoveUIAndSetInvalid();
	void ResetAfterThrowing();
	void DelayReset();
};

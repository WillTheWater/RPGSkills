// WillTheWater

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGSkillsBaseCharacter.generated.h"

UCLASS()
class RPGSKILLS_API ARPGSkillsBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ARPGSkillsBaseCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USpringArmComponent> CameraBoom;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UCameraComponent> FollowCamera;

	UPROPERTY(EditAnywhere)
	class UInputMappingContext* IMC_RPGSkills;

protected:
	virtual void BeginPlay() override;
};

// WillTheWater

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGSkillsBaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

UENUM(BlueprintType)
enum class EMovementTypes : uint8
{
	MT_EMAX UMETA(DisplayName = "EMAX"),
	MT_WALKING UMETA(DisplayName = "Walking"),
	MT_EXHAUSTED UMETA(DisplayName = "Exhuated"),
	MT_SPRINTING UMETA(DisplayName = "Sprinting"),
	MT_GLIDING UMETA(DisplayName = "Gliding"),
	MT_FALLING UMETA(DisplayName = "Falling")
};

UCLASS()
class RPGSKILLS_API ARPGSkillsBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ARPGSkillsBaseCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void LocomotionManager(EMovementTypes NewMovementType);

	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void MoveReleased(const FInputActionValue& Value);

	UFUNCTION()
	void Look(const FInputActionValue& Value);

	UFUNCTION()
	void Sprint(const FInputActionValue& Value);

	UFUNCTION()
	void SprintReleased(const FInputActionValue& Value);

	UFUNCTION()
	void SprintStarted(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* IMC_RPGSkills;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, Category = "CharacterMovementState")
	EMovementTypes CurrentMT = EMovementTypes::MT_EMAX;


	float VelocityX;
	float VelocityY;

protected:
	virtual void BeginPlay() override;
};

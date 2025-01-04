// WillTheWater

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGSkillsBaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class URPGOverlayUI;

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

	UFUNCTION()
	void JumpGlideStarted(const FInputActionValue& Value);

	UFUNCTION()
	void JumpGlideReleased(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	bool const IsCharacterExausted();

	void SetSprint();
	void ResetToWalk();
	void SetWalking();
	void SetExhausted();
	void SetGliding();
	void SetFalling();

	void DrainStaminaTimer();
	FTimerHandle DrainStaminaTimerHandle;

	void StartStaminaDrain();
	void RecoverStaminaTimer();
	FTimerHandle RecoverStaminaTimerHandle;
	void StartStaminaRecovery();

	void ClearStaminaTimers();
	void AddGravityTimer();
	FTimerHandle AddGravityTimerHandle;

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

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpGlideAction;

	UPROPERTY(EditAnywhere, Category = "CharacterMovementState")
	EMovementTypes CurrentMT = EMovementTypes::MT_EMAX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float CurrentStamina = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaLossRate = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaCost = 0.5f;

	UPROPERTY(EditAnywhere, Category = "UserInterface")
	TSubclassOf<UUserWidget> OverlayClassReference;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	FVector EnableGlideDistance = FVector(0.f, 0.f, -150.f);

	float VelocityX;
	float VelocityY;

	TObjectPtr<URPGOverlayUI> UIReference;

protected:
	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult& Hit) override;
};

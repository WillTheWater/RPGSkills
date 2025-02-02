// WillTheWater

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGSkillsBaseCharacter.generated.h"

class ABombBase;
struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class URPGOverlayUI;
class USkeletalMeshComponent;

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

UENUM(BlueprintType)
enum class ESkills : uint8
{
	SK_EMAX UMETA(DisplayName = EMAX),
	SK_RBB UMETA(DisplayName = RBB),
	SK_RBS UMETA(DisplayName = RBS),
	SK_MAG UMETA(DisplayName = MAG),
	SK_STASIS UMETA(DisplayName = STASIS),
	SK_ICE UMETA(DisplayName = ICE)
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

	void DeavtivateAllSkills();
	
	UFUNCTION()
	void ToggleUIStarted(const FInputActionValue& Value);

	UFUNCTION()
	void PrepareSkillStarted(const FInputActionValue& Value);

	UFUNCTION()
	void CastSkillStarted(const FInputActionValue& Value);

	UFUNCTION()
	void ToggleSkillActivity();

	UFUNCTION()
	void ToggleRemoteBomb();

	UFUNCTION(BlueprintCallable)
	bool const IsCharacterExausted();

	UFUNCTION(BlueprintImplementableEvent)
	int32 GetWidgetSwitcherInfo();

	UFUNCTION(BlueprintImplementableEvent)
	void SetWidgetSwitcherIndex(int32 Index);

	UFUNCTION(BlueprintImplementableEvent)
	void CrosshairAndCameraMode(bool bShow);

	void SetSprint();
	void ResetToWalk();
	void SetWalking();
	void SetExhausted();
	void SetGliding();
	void SetFalling();

	void ThrowAndIgniteBomb(bool bSphere);
	const FVector GetThrowDirection();

	void DrainStaminaTimer();
	FTimerHandle DrainStaminaTimerHandle;

	void StartStaminaDrain();
	void RecoverStaminaTimer();
	FTimerHandle RecoverStaminaTimerHandle;
	void StartStaminaRecovery();

	void ClearStaminaTimers();
	void AddGravityTimer();
	FTimerHandle AddGravityTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> Glider;

	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> BombReadyPosition;
	
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

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ToggleUIAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* PrepareSkillAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* CastSkillAction;

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

	UPROPERTY(EditAnywhere, Category = "Skills")
	ESkills ActiveSkill{ ESkills::SK_EMAX };

	UPROPERTY(EditAnywhere, Category = "Skills")
	TSubclassOf<ABombBase> SphereBombClass;

	UPROPERTY(EditAnywhere, Category = "Skills")
	TSubclassOf<ABombBase> BoxBombClass;
	
	float VelocityX;
	float VelocityY;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UserInterface")
	TObjectPtr<URPGOverlayUI> UIReference;

	bool bReadyToThrow = false;
	bool bShowCrosshair = false;
	bool bRBActivated = false;
	bool bMAGActivated = false;
	bool bStasisActivated = false;
	bool bIceActivated = false;
	bool bHandleBomb = false;
	TObjectPtr<ABombBase> BombReference;

protected:
	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult& Hit) override;
};

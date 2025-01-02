// WillTheWater

#include "Characters/RPGSkillsBaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Controllers/RPGSkillsPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/RPGOverlayUI.h"

ARPGSkillsBaseCharacter::ARPGSkillsBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void ARPGSkillsBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	ARPGSkillsPlayerController* PC = Cast<ARPGSkillsPlayerController>(Controller);
	if (PC == nullptr) { return; }

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (Subsystem == nullptr) { return; }

	Subsystem->AddMappingContext(IMC_RPGSkills, 0);

	CurrentStamina = MaxStamina;

	if (OverlayClassReference)
	{
		UIReference = CreateWidget<URPGOverlayUI>(GetWorld(), OverlayClassReference);
		if (UIReference)
		{
			UIReference->PlayerReference = this;
			UIReference->AddToViewport();
		}
	}
}

void ARPGSkillsBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FString StaminaStr = FString::SanitizeFloat(CurrentStamina);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, StaminaStr);

}

void ARPGSkillsBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EIComponent == nullptr) { return; }
	EIComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARPGSkillsBaseCharacter::Move);
	EIComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ARPGSkillsBaseCharacter::MoveReleased);
	EIComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARPGSkillsBaseCharacter::Look);
	EIComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ARPGSkillsBaseCharacter::Sprint);
	EIComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ARPGSkillsBaseCharacter::SprintReleased);
	EIComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ARPGSkillsBaseCharacter::SprintStarted);

}

void ARPGSkillsBaseCharacter::LocomotionManager(EMovementTypes NewMovementType)
{
	if (CurrentMT == NewMovementType) { return; }
	else { CurrentMT = NewMovementType; }

	if (CurrentMT == EMovementTypes::MT_GLIDING)
	{
		//TODO
	}

	switch (CurrentMT)
	{
	case EMovementTypes::MT_EMAX:
		break;
	case EMovementTypes::MT_WALKING:
		SetWalking();
		break;
	case EMovementTypes::MT_EXHAUSTED:
		break;
	case EMovementTypes::MT_SPRINTING:
		SetSprint();
		break;
	case EMovementTypes::MT_GLIDING:
		break;
	case EMovementTypes::MT_FALLING:
		break;
	default:
		break;
	}
}

void ARPGSkillsBaseCharacter::Move(const FInputActionValue& Value)
{
	FVector2D VectorData = Value.Get<FVector2D>();
	VelocityX = VectorData.X;
	VelocityY = VectorData.Y;

	if (Controller == nullptr) { return; }
	FRotator YawRotator = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);


	FVector WorldDirectionX = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::X);
	FVector WorldDirectionY = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::Y);
	AddMovementInput(WorldDirectionX, VelocityX);
	AddMovementInput(WorldDirectionY, VelocityY);
}

void ARPGSkillsBaseCharacter::MoveReleased(const FInputActionValue& Value)
{
	VelocityX = 0.f;
	VelocityY = 0.f;
}

void ARPGSkillsBaseCharacter::Look(const FInputActionValue& Value)
{
	FVector2D ControllerRotation = Value.Get<FVector2D>();
	AddControllerYawInput(ControllerRotation.X);
	AddControllerPitchInput(ControllerRotation.Y);
}

void ARPGSkillsBaseCharacter::Sprint(const FInputActionValue& Value)
{
	if (CurrentMT == EMovementTypes::MT_SPRINTING && VelocityX == 0.f && VelocityY == 0.f)
	{
		LocomotionManager(EMovementTypes::MT_WALKING);
	}
}

void ARPGSkillsBaseCharacter::SprintReleased(const FInputActionValue& Value)
{
	if (CurrentMT == EMovementTypes::MT_SPRINTING)
	{
		LocomotionManager(EMovementTypes::MT_WALKING);
	}
}

void ARPGSkillsBaseCharacter::SprintStarted(const FInputActionValue& Value)
{
	if (CurrentMT == EMovementTypes::MT_EMAX || CurrentMT == EMovementTypes::MT_WALKING)
	{
		LocomotionManager(EMovementTypes::MT_SPRINTING);
	}
}

bool const ARPGSkillsBaseCharacter::IsCharacterExausted()
{
	bool bEqual = CurrentMT == EMovementTypes::MT_EXHAUSTED;
	return bEqual;
}

void ARPGSkillsBaseCharacter::SetSprint()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, "Sprinting");
	GetCharacterMovement()->MaxWalkSpeed = 1000.f;
	GetCharacterMovement()->AirControl = 0.35f;
	ResetToWalk();
	StartStaminaDrain();
}

void ARPGSkillsBaseCharacter::ResetToWalk()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void ARPGSkillsBaseCharacter::SetWalking()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, "Set Walking");
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->AirControl = 0.05f;
	ResetToWalk();
	StartStaminaRecovery();
}

void ARPGSkillsBaseCharacter::DrainStaminaTimer()
{
	if (CurrentStamina <= 0.f)
	{
		LocomotionManager(EMovementTypes::MT_EXHAUSTED);
	}
	else
	{
		CurrentStamina = FMath::Clamp((CurrentStamina - StaminaLossRate), 0.f, MaxStamina);
	}
}

void ARPGSkillsBaseCharacter::StartStaminaDrain()
{
	ClearStaminaTimers();
	GetWorldTimerManager().SetTimer(DrainStaminaTimerHandle, this, &ARPGSkillsBaseCharacter::DrainStaminaTimer, StaminaLossRate, true);

	if (UIReference)
	{
		UIReference->ShowGaugeAnim(true);
	}
}

void ARPGSkillsBaseCharacter::RecoverStaminaTimer()
{
	if (CurrentStamina < MaxStamina)
	{
		CurrentStamina = FMath::Clamp((CurrentStamina + StaminaLossRate), 0.f, MaxStamina);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(RecoverStaminaTimerHandle);
		LocomotionManager(EMovementTypes::MT_WALKING);

		if (UIReference)
		{
			UIReference->ShowGaugeAnim(false);
		}
	}
}

void ARPGSkillsBaseCharacter::StartStaminaRecovery()
{
	ClearStaminaTimers();
	GetWorldTimerManager().SetTimer(RecoverStaminaTimerHandle, this, &ARPGSkillsBaseCharacter::RecoverStaminaTimer, StaminaLossRate, true);
}

void ARPGSkillsBaseCharacter::ClearStaminaTimers()
{
	GetWorldTimerManager().ClearTimer(DrainStaminaTimerHandle);
	GetWorldTimerManager().ClearTimer(RecoverStaminaTimerHandle);
}


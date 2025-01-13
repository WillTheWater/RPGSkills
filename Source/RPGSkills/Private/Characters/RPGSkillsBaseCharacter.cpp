// WillTheWater

#include "Characters/RPGSkillsBaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Controllers/RPGSkillsPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/RPGOverlayUI.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"

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

	Glider = CreateDefaultSubobject<USkeletalMeshComponent>("Glider");
	Glider->SetupAttachment(GetMesh());
	Glider->SetVisibility(false);
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

void ARPGSkillsBaseCharacter::Landed(const FHitResult& Hit)
{
	if (CurrentMT == EMovementTypes::MT_EXHAUSTED)
	{
		StartStaminaRecovery();
		return;
	}
	if (CurrentMT == EMovementTypes::MT_GLIDING)
	{
		LocomotionManager(EMovementTypes::MT_WALKING);
		return;
	}

	LocomotionManager(EMovementTypes::MT_WALKING);

}

void ARPGSkillsBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FString StaminaStr = FString::SanitizeFloat(CurrentStamina);
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, StaminaStr);

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
	EIComponent->BindAction(JumpGlideAction, ETriggerEvent::Completed, this, &ARPGSkillsBaseCharacter::JumpGlideReleased);
	EIComponent->BindAction(JumpGlideAction, ETriggerEvent::Started, this, &ARPGSkillsBaseCharacter::JumpGlideStarted);
	EIComponent->BindAction(ToggleUIAction, ETriggerEvent::Started, this, &ARPGSkillsBaseCharacter::ToggleUIStarted);
	EIComponent->BindAction(PrepareSkillAction, ETriggerEvent::Started, this, &ARPGSkillsBaseCharacter::PrepareSkillStarted);
	EIComponent->BindAction(CastSkillAction, ETriggerEvent::Started, this, &ARPGSkillsBaseCharacter::CastSkillStarted);

}

void ARPGSkillsBaseCharacter::LocomotionManager(EMovementTypes NewMovementType)
{
	if (CurrentMT == NewMovementType) { return; }
	else { CurrentMT = NewMovementType; }

	if (Glider)
	{
		Glider->SetVisibility(CurrentMT == EMovementTypes::MT_GLIDING);
	}

	switch (CurrentMT)
	{
	case EMovementTypes::MT_EMAX:
		break;
	case EMovementTypes::MT_WALKING:
		SetWalking();
		break;
	case EMovementTypes::MT_EXHAUSTED:
		SetExhausted();
		break;
	case EMovementTypes::MT_SPRINTING:
		SetSprint();
		break;
	case EMovementTypes::MT_GLIDING:
		SetGliding();
		break;
	case EMovementTypes::MT_FALLING:
		SetFalling();
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

void ARPGSkillsBaseCharacter::JumpGlideStarted(const FInputActionValue& Value)
{
	if (CurrentMT == EMovementTypes::MT_EXHAUSTED) { return; }
	if (GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Falling)
	{
		Jump();
		LocomotionManager(EMovementTypes::MT_FALLING);
		return;
	}

	if (CurrentMT == EMovementTypes::MT_GLIDING)
	{
		LocomotionManager(EMovementTypes::MT_FALLING);
		return;
	}

	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start + EnableGlideDistance;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 5.f, 0, 3.f);
	if (bHit)
	{

	}
	else
	{
		LocomotionManager(EMovementTypes::MT_GLIDING);
	}
}

void ARPGSkillsBaseCharacter::JumpGlideReleased(const FInputActionValue& Value)
{
	StopJumping();
}

void ARPGSkillsBaseCharacter::ToggleUIStarted(const FInputActionValue& Value)
{
	// TODO Cancel Cast

	ARPGSkillsPlayerController* PC = Cast<ARPGSkillsPlayerController>(Controller);
	if (GetWigdetSwitcherIndex() == 1)
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetPause(false);
		SetWidgetSwitcherIndex(0);
	}
	else
	{
		PC->bShowMouseCursor = true;
		FInputModeGameAndUI InputHandle;
		InputHandle.SetWidgetToFocus(UIReference->TakeWidget());
		PC->SetInputMode(InputHandle);
		PC->SetPause(true);
		SetWidgetSwitcherIndex(1);
	}
}

void ARPGSkillsBaseCharacter::ToggleUIReleased(const FInputActionValue& Value)
{
}

void ARPGSkillsBaseCharacter::PrepareSkillStarted(const FInputActionValue& Value)
{
	ToggleSkillActivity();
}

void ARPGSkillsBaseCharacter::CastSkillStarted(const FInputActionValue& Value)
{
	switch (ActiveSkill)
	{
	case ESkills::SK_EMAX:
		break;
	case ESkills::SK_RBB:
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Trigger Bomb");
		break;
	case ESkills::SK_RBS:
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Trigger Remote");
		break;
	case ESkills::SK_MAG:
		break;
	case ESkills::SK_STASIS:
		break;
	case ESkills::SK_ICE:
		break;
	default:
		break;
	}
}

void ARPGSkillsBaseCharacter::ToggleSkillActivity()
{
	if (ActiveSkill == ESkills::SK_EMAX) { return; }
	bShowCrosshair = !bShowCrosshair;
	CrosshairAndCameraMode(bShowCrosshair);
	LocomotionManager(EMovementTypes::MT_WALKING);
	switch (ActiveSkill)
	{
	case ESkills::SK_EMAX:
		break;
	case ESkills::SK_RBB:
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Spawn Bomb");
		break;
	case ESkills::SK_RBS:
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Spawn Remote");
		break;
	case ESkills::SK_MAG:
		break;
	case ESkills::SK_STASIS:
		break;
	case ESkills::SK_ICE:
		break;
	default:
		break;
	}
}

bool const ARPGSkillsBaseCharacter::IsCharacterExausted()
{
	bool bEqual = CurrentMT == EMovementTypes::MT_EXHAUSTED;
	return bEqual;
}

void ARPGSkillsBaseCharacter::SetSprint()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, "Sprinting");
	GetCharacterMovement()->MaxWalkSpeed = 1000.f;
	GetCharacterMovement()->AirControl = 0.35f;
	ResetToWalk();
	StartStaminaDrain();
}

void ARPGSkillsBaseCharacter::ResetToWalk()
{
	GetWorldTimerManager().ClearTimer(AddGravityTimerHandle);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void ARPGSkillsBaseCharacter::SetWalking()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, "Set Walking");
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->AirControl = 0.05f;
	ResetToWalk();
	StartStaminaRecovery();
}

void ARPGSkillsBaseCharacter::SetExhausted()
{
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->AirControl = 0.35;
	ClearStaminaTimers();
	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
	{
		StartStaminaRecovery();
	}
	else if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling)
	{
		ResetToWalk();
	}
	else { return; }
}

void ARPGSkillsBaseCharacter::SetGliding()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->AirControl = 0.6f;
	StartStaminaDrain();
	GetWorldTimerManager().SetTimer(AddGravityTimerHandle, this, &ARPGSkillsBaseCharacter::AddGravityTimer, GetWorld()->GetDeltaSeconds(), true);
}

void ARPGSkillsBaseCharacter::SetFalling()
{
	ClearStaminaTimers();
	ResetToWalk();
	GetCharacterMovement()->AirControl = 0.35f;
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

void ARPGSkillsBaseCharacter::AddGravityTimer()
{
	LaunchCharacter(FVector(0.f, 0.f, - 100.f), false, true);
}


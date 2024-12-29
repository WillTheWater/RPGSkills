// WillTheWater

#include "Characters/RPGSkillsBaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Controllers/RPGSkillsPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

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
}

void ARPGSkillsBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARPGSkillsBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EIComponent == nullptr) { return; }
	EIComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARPGSkillsBaseCharacter::Move);
	EIComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ARPGSkillsBaseCharacter::MoveReleased);

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
}


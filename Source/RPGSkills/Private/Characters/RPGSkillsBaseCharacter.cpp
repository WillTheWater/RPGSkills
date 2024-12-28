// WillTheWater

#include "Characters/RPGSkillsBaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Controllers/RPGSkillsPlayerController.h"
#include "EnhancedInputSubsystems.h"

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

}


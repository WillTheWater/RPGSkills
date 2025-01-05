// WillTheWater


#include "Animation/RPGAnimInstance.h"
#include "Characters/RPGSkillsBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void URPGAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerReference = Cast<ARPGSkillsBaseCharacter>(TryGetPawnOwner());
	if (PlayerReference == nullptr) { return; }
	MovementComponent = PlayerReference->GetCharacterMovement();
}

void URPGAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	
	if (!PlayerReference || !MovementComponent) { return; }
	GroundSpeed = UKismetMathLibrary::VSizeXY(PlayerReference->GetVelocity());
	AirSpeed = PlayerReference->GetVelocity().Z;
	bIsFalling = MovementComponent->IsFalling();
	bShouldMove = !bIsFalling && GroundSpeed >= 5.f && MovementComponent->GetCurrentAcceleration().Size() > 0;
	bIsGliding = PlayerReference->CurrentMT == EMovementTypes::MT_GLIDING;
	bReadyToThrow = PlayerReference->bReadyToThrow;
}

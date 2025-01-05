// WillTheWater

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RPGAnimInstance.generated.h"

class ARPGSkillsBaseCharacter;
class UCharacterMovementComponent;

UCLASS()
class RPGSKILLS_API URPGAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	TObjectPtr<ARPGSkillsBaseCharacter> PlayerReference;
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	float GroundSpeed = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	float AirSpeed = 0.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	bool bShouldMove = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	bool bIsFalling = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	bool bIsGliding = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	bool bReadyToThrow = false;
};

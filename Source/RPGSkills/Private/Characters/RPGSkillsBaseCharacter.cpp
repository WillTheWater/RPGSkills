// WillTheWater


#include "Characters/RPGSkillsBaseCharacter.h"

// Sets default values
ARPGSkillsBaseCharacter::ARPGSkillsBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARPGSkillsBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARPGSkillsBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARPGSkillsBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


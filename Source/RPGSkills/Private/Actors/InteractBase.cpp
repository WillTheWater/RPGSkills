// WillTheWater


#include "Actors/InteractBase.h"

#include "Components/SphereComponent.h"

AInteractBase::AInteractBase()
{
	PrimaryActorTick.bCanEverTick = false;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	InteractSphere = CreateDefaultSubobject<USphereComponent>("Interact Sphere");
	InteractSphere->SetupAttachment(BaseMesh);
	InteractSphere->SetSphereRadius(150.f);
	InteractSphere->OnComponentBeginOverlap.AddDynamic(this, &AInteractBase::OnBeginOverlap);
	InteractSphere->OnComponentEndOverlap.AddDynamic(this, &AInteractBase::OnEndOverlap);
}

void AInteractBase::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(FName("Tag.Player")))
	{
		PlayerEntered();
		PlayerEnderedCPP();
	}
}

void AInteractBase::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(FName("Tag.Player")))
	{
		PlayerLeft();
		PlayerLeftCPP();
	}
}

void AInteractBase::PlayerEnderedCPP()
{
	
}

void AInteractBase::PlayerLeftCPP()
{
}

void AInteractBase::ToggleIntereaction(ARPGSkillsBaseCharacter* PlayerReference)
{
	
}

void AInteractBase::NextActionInteractActor()
{
}

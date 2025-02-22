// WillTheWater


#include "Actors/PickupActor.h"

#include "Blueprint/UserWidget.h"
#include "Characters/RPGSkillsBaseCharacter.h"
#include "Components/SphereComponent.h"

APickupActor::APickupActor()
{
		
}

void APickupActor::PlayerEnderedCPP()
{
	if (PickupWidgetClass == nullptr) {return;}
	if (PickUI == nullptr)
	{
		PickUI = CreateWidget<UUserWidget>(GetWorld(), PickupWidgetClass);
		PickUI->AddToViewport();
	}
}

void APickupActor::PlayerLeftCPP()
{
	if (PickUI != nullptr)
	{
		PickUI->RemoveFromParent();
		PickUI = nullptr;
	}
}

void APickupActor::ToggleIntereaction(ARPGSkillsBaseCharacter* PlayerReference)
{
	if (!PlayerReference) {return;}
	RPGPlayerReference = PlayerReference;
	RemoveUIAndSetInvalid();
	SetActorEnableCollision(false);
	BaseMesh->SetSimulatePhysics(false);
	BaseMesh->AttachToComponent(PlayerReference->BombReadyPosition, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	PlayerReference->CrosshairAndCameraMode(true);
	PlayerReference->bReadyToThrow = true;
}

void APickupActor::NextActionInteractActor()
{
	if (RPGPlayerReference)
	{
		RPGPlayerReference->CancelReadyToThrow(BaseMesh);
		SetActorEnableCollision(true);
		ResetAfterThrowing();
		RPGPlayerReference->bReadyToThrow = false;
		RPGPlayerReference = nullptr;
	}
}

void APickupActor::RemoveUIAndSetInvalid()
{
	if (PickUI)
	{
		PickUI->RemoveFromParent();
		PickUI = nullptr;
	}
	InteractSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APickupActor::ResetAfterThrowing()
{
	GetWorld()->GetTimerManager().SetTimer(DelayResetTimer, this, &APickupActor::DelayReset, 2.f, false);
}

void APickupActor::DelayReset()
{
	InteractSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

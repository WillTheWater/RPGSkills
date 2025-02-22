// WillTheWater


#include "Actors/PressurePlate.h"

#include "Components/BoxComponent.h"

APressurePlate::APressurePlate()
{
	PrimaryActorTick.bCanEverTick = true;
	SceneRoot = CreateDefaultSubobject<USceneComponent>("Scene Root");
	RootComponent = SceneRoot;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("Box Component");
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh Component");
	StaticMeshComponent->SetupAttachment(SceneRoot);
	ButtonMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Button Component");
	ButtonMeshComponent->SetupAttachment(SceneRoot);
}

void APressurePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


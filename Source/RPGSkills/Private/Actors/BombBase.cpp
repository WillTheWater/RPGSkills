// WillTheWater

#include "Actors/BombBase.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

ABombBase::ABombBase()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Bomb Mesh");
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetLinearDamping(0.35f);
}

void ABombBase::Detonate()
{
	
}

void ABombBase::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BombSpawn, GetActorLocation());
}

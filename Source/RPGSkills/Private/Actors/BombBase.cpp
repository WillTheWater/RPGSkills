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
	GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::MakeRandomColor(),TEXT("Detonated"));
	
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BombExplosionVFX, MeshComponent->GetComponentLocation());
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), BombExplosionSFX, GetActorLocation());
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayWorldCameraShake(GetWorld(), BombExplosionShake, MeshComponent->GetComponentLocation(), 0.f, 2000.f, false);

	Destroy();
}

void ABombBase::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BombSpawn, GetActorLocation());
}

// WillTheWater

#include "Actors/BombBase.h"

#include "Actors/WindTunnel.h"
#include "Components/StaticMeshComponent.h"
#include "Evaluation/Blending/MovieSceneBlendType.h"
#include "Field/FieldSystemActor.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"

ABombBase::ABombBase()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Bomb Mesh");
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetLinearDamping(0.35f);

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>("Radial Force Component");
	RadialForceComponent->SetupAttachment(RootComponent);
	RadialForceComponent->Radius = 300.0f;
	RadialForceComponent->ImpulseStrength = 3000.0f;
	RadialForceComponent->bImpulseVelChange = true;
	RadialForceComponent->ForceStrength = 100.f;
}

void ABombBase::Detonate()
{
	GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::MakeRandomColor(),TEXT("Detonated"));
	
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BombExplosionVFX, MeshComponent->GetComponentLocation());
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), BombExplosionSFX, GetActorLocation());
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayWorldCameraShake(GetWorld(), BombExplosionShake, MeshComponent->GetComponentLocation(), 0.f, 2000.f, false);

	if (FieldSystemActorClass)
	{
		GetWorld()->SpawnActor<AActor>(FieldSystemActorClass, MeshComponent->GetComponentLocation(), FRotator::ZeroRotator);
	}
	RadialForceComponent->FireImpulse();

	FVector Start = MeshComponent->GetComponentLocation();
	FVector End = Start;
	TArray<FHitResult> Hits;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(200.f);
	FCollisionQueryParams Params;
	Params.bReturnPhysicalMaterial = true;
	GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, ECC_Visibility, Sphere, Params);

	bool MakeWind = false;
	for (auto hit : Hits)
	{
		if (hit.bBlockingHit && hit.PhysMaterial != nullptr)
		{
			switch (UGameplayStatics::GetSurfaceType(hit))
			{
				case SurfaceType1:
					break;
				case SurfaceType2:
					MakeWind = true;
					break;
			}
		}
	}

	if (MakeWind && WindTunnelClass)
	{
		FVector WindLocation = FVector(MeshComponent->GetComponentLocation().X, MeshComponent->GetComponentLocation().Y, MeshComponent->GetComponentLocation().Z + 800.f);
		FTransform WindTransform(FRotator::ZeroRotator, WindLocation, FVector(4,4,4));
		AWindTunnel* NewWindTunnel = GetWorld()->SpawnActorDeferred<AWindTunnel>(WindTunnelClass, WindTransform);
		NewWindTunnel->bTempWind = true;
		NewWindTunnel->SetLifeSpan(10.f);
		NewWindTunnel->FinishSpawning(WindTransform);
	}
	
	Destroy();
}

void ABombBase::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BombSpawn, GetActorLocation());
}

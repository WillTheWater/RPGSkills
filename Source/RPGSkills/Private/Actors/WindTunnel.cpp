// WillTheWater

#include "Actors/WindTunnel.h"
#include "Characters/RPGSkillsBaseCharacter.h"
#include "Components/BoxComponent.h"

AWindTunnel::AWindTunnel()
{
	PrimaryActorTick.bCanEverTick = true;
	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	Box->SetupAttachment(RootComponent);
	Box->OnComponentBeginOverlap.AddDynamic(this, &AWindTunnel::OnOverlapBegin);
	Box->OnComponentEndOverlap.AddDynamic(this, &AWindTunnel::OnOverlapEnd);
}

void AWindTunnel::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PlayerReference = Cast<ARPGSkillsBaseCharacter>(OtherActor);
}

void AWindTunnel::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	PlayerReference = nullptr;
}

void AWindTunnel::BeginPlay()
{
	Super::BeginPlay();

	if (bTempWind)
	{
		InitialLifeSpan = 30.f;
	}
	
}

void AWindTunnel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerReference == nullptr) { return; }
	if (PlayerReference->CurrentMT != EMovementTypes::MT_GLIDING) { return; }

	FVector UpVector = GetActorUpVector() * 10.f;
	PlayerReference->AddActorWorldOffset(UpVector);
}


// WillTheWater

#include "Actors/Ice.h"

#include "Components/BoxComponent.h"

AIce::AIce()
{
	PrimaryActorTick.bCanEverTick = true;

	BaseSceneRoot = CreateDefaultSubobject<USceneComponent>("Base Scene Root");
	BaseSceneRoot->SetupAttachment(RootComponent);

	IceMesh = CreateDefaultSubobject<UStaticMeshComponent>("Ice Mesh");
	IceMesh->SetupAttachment(BaseSceneRoot);
	IceMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	IceMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	IceMesh->SetCollisionObjectType(ECC_WorldStatic);
	IceMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CheckOverlapComp = CreateDefaultSubobject<UBoxComponent>("Check Overlap");
	CheckOverlapComp->SetupAttachment(BaseSceneRoot);
	CheckOverlapComp->SetCollisionObjectType(ECC_WorldStatic);

	SolidBoxComp = CreateDefaultSubobject<UBoxComponent>("Solid Box");
	SolidBoxComp->SetupAttachment(BaseSceneRoot);
	SolidBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AIce::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AIce::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


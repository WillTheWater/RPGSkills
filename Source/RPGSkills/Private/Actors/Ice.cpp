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
	SolidBoxComp->SetCollisionObjectType(ECC_WorldStatic);
	SolidBoxComp->SetCollisionResponseToAllChannels(ECR_Block);
	SolidBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AIce::BeginPlay()
{
	Super::BeginPlay();
	if (CollisionCurve)
	{
		FOnTimelineFloat TFCollisionHandle;
		TFCollisionHandle.BindUFunction(this, FName("CollisionUpdate"));

		CollisionTimeline.AddInterpFloat(CollisionCurve, TFCollisionHandle);
		ExtendStart = SolidBoxComp->GetScaledBoxExtent();
		ExtendEnd = FVector(SolidBoxComp->GetScaledBoxExtent().X, SolidBoxComp->GetScaledBoxExtent().Y, 100.f);
		RelativeStart = SolidBoxComp->GetRelativeLocation();
		RelativeEnd = FVector(SolidBoxComp->GetRelativeLocation().X, SolidBoxComp->GetRelativeLocation().Y, 100.f);
		
	}
}

// Called every frame
void AIce::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CollisionTimeline.TickTimeline(DeltaTime);
}


bool AIce::CheckOverlapping()
{
	TArray<FHitResult> Hits;
	FVector Start = CheckOverlapComp->GetComponentLocation();
	FVector End = Start;
	FQuat Rot = CheckOverlapComp->GetComponentRotation().Quaternion();
	FCollisionShape Collision = FCollisionShape::MakeBox(CheckOverlapComp->GetScaledBoxExtent());
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	GetWorld()->SweepMultiByChannel(Hits, Start, End, Rot, ECC_Visibility, Collision, Params);

	for (auto Hit : Hits)
	{
		ECollisionChannel TempObject = Hit.GetComponent()->GetCollisionObjectType();
		//if (TempObject != ECC_GameTraceChannel1)
		if (TempObject == ECC_WorldStatic)
		{
			return false;
		}
	}
	return true;
	
}

void AIce::SpawnIce()
{
	if (!bCanPlace) {return;}
	AIce* IcePlatform = GetWorld()->SpawnActor<AIce>(IceClass, GetActorLocation(), GetActorRotation());
	if (!IcePlatform) {return;}
	IcePlatform->EnableCollision();
}

void AIce::EnableCollision()
{
	SolidBoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionTimeline.PlayFromStart();
}

void AIce::CollisionUpdate(float DeltaTime)
{
	FVector NewBoxExtend = FMath::Lerp(ExtendStart, ExtendEnd, DeltaTime);
	SolidBoxComp->SetBoxExtent(NewBoxExtend);
	FVector NewRelativeLocation = FMath::Lerp(RelativeStart, RelativeEnd, DeltaTime);
	SolidBoxComp->SetRelativeLocation(NewRelativeLocation);
}

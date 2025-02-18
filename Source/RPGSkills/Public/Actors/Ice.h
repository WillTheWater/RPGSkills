// WillTheWater

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include  "Components/TimelineComponent.h"
#include "Ice.generated.h"

class UBoxComponent;
class UCurveFloat;
struct FTimeline;

UCLASS()
class RPGSKILLS_API AIce : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIce();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> BaseSceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> IceMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBoxComponent> CheckOverlapComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBoxComponent> SolidBoxComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AIce> IceClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* CollisionCurve;

	bool bCanPlace = false;
	FVector ExtendStart;
	FVector ExtendEnd;
	FVector RelativeStart;
	FVector RelativeEnd;
	FTimeline CollisionTimeline;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	bool CheckOverlapping();
	void SpawnIce();
	void EnableCollision();

	UFUNCTION()
	void CollisionUpdate(float DeltaTime);

};


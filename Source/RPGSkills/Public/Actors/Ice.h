// WillTheWater

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ice.generated.h"

class UBoxComponent;

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

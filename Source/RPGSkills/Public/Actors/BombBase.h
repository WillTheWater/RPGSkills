// WillTheWater

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BombBase.generated.h"

class UStaticMeshComponent;
class UParticleSystem;
class UParticleSystemComponent;

UCLASS()
class RPGSKILLS_API ABombBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ABombBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UParticleSystem> BombSpawn;

protected:
	virtual void BeginPlay() override;
};

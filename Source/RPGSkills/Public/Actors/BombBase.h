// WillTheWater

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BombBase.generated.h"

class UStaticMeshComponent;
class UParticleSystem;
class UParticleSystemComponent;
class USoundBase;
class UCameraShakeBase;

UCLASS()
class RPGSKILLS_API ABombBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ABombBase();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assets")
	TObjectPtr<UParticleSystem> BombSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assets")
	TObjectPtr<UParticleSystem> BombExplosionVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assets")
	TObjectPtr<USoundBase> BombExplosionSFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assets")
	TSubclassOf<UCameraShakeBase> BombExplosionShake;
	
	void Detonate();
protected:
	virtual void BeginPlay() override;
};

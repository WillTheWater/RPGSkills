// WillTheWater

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PressurePlate.generated.h"

class USceneComponent;
class UBoxComponent;

UCLASS()
class RPGSKILLS_API APressurePlate : public AActor
{
	GENERATED_BODY()
	
public:	
	APressurePlate();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> SceneRoot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> ButtonMeshComponent;
	
	virtual void Tick(float DeltaTime) override;

};

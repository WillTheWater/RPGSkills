// WillTheWater

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractBase.generated.h"

class ARPGSkillsBaseCharacter;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class RPGSKILLS_API AInteractBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AInteractBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interact)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interact)
	USphereComponent* InteractSphere;

	ARPGSkillsBaseCharacter* RPGPlayerReference;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION(BlueprintImplementableEvent)
	void PlayerEntered();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayerLeft();
	
	virtual void PlayerEnderedCPP();
	virtual void PlayerLeftCPP();
	virtual void ToggleIntereaction(ARPGSkillsBaseCharacter* PlayerReference);
	virtual void NextActionInteractActor();
};

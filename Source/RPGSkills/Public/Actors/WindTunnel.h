// WillTheWater

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WindTunnel.generated.h"

class ARPGSkillsBaseCharacter;
class UBoxComponent;

UCLASS()
class RPGSKILLS_API AWindTunnel : public AActor
{
	GENERATED_BODY()
	
public:	
	AWindTunnel();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTempWind = false;

	UPROPERTY(EditAnywhere)
	ARPGSkillsBaseCharacter* PlayerReference;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Box;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	virtual void BeginPlay() override;

};

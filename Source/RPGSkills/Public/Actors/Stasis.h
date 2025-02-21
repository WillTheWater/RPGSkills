// WillTheWater

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Stasis.generated.h"

class UArrowComponent;

UCLASS()
class RPGSKILLS_API AStasis : public AActor
{
	GENERATED_BODY()
	
public:	
	AStasis();

	UPROPERTY(EditAnywhere, blueprintReadWrite)
	TObjectPtr<UArrowComponent> IndicatorArrow;

	FVector GetImpulse();
	void UpdateForceInfo();

	int32 Impulse = 0;
	int32 HitCount = 0;

	FORCEINLINE UArrowComponent* GetIndicatorArrow() const { return IndicatorArrow; }
	
	
};

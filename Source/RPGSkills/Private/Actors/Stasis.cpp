// WillTheWater


#include "Actors/Stasis.h"

#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"

AStasis::AStasis()
{
	PrimaryActorTick.bCanEverTick = true;
	IndicatorArrow = CreateDefaultSubobject<UArrowComponent>("Indicator Arrow");
	IndicatorArrow->SetupAttachment(RootComponent);
	IndicatorArrow->SetHiddenInGame(false);
}

FVector AStasis::GetImpulse()
{
	FVector AddForce = UKismetMathLibrary::Conv_RotatorToVector(IndicatorArrow->GetRelativeRotation());
	return AddForce * Impulse;
}

void AStasis::UpdateForceInfo()
{
	HitCount = FMath::Clamp(HitCount + 1, 0, 5);
	FVector TempScale = FVector(HitCount + 1, 0.f, 0.f);
	IndicatorArrow->SetRelativeScale3D(TempScale);

	float TempScaleX = IndicatorArrow->GetRelativeScale3D().X;
	float AlphaColor = UKismetMathLibrary::MapRangeClamped(TempScaleX, 1.f, 5.f, 0.f, 1.f);
	FLinearColor TempColor = FLinearColor::LerpUsingHSV(FLinearColor::Yellow, FLinearColor::Red, AlphaColor);
	IndicatorArrow->SetArrowColor(TempColor);
	Impulse = HitCount * 2500.f;
}

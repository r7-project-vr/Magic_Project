// Fill out your copyright notice in the Description page of Project Settings.


#include "Sato/PlayerWayRoad.h"
#include "Components/SplineComponent.h"
#include "Math/UnrealMathUtility.h"

class USplineComponent;

// Sets default values
APlayerWayRoad::APlayerWayRoad()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	Spline->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APlayerWayRoad::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerWayRoad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

// 
FTransform APlayerWayRoad::GetSplineTransform(float& distance, float speed)
{
	// Wrap関数に使う変数たち
	WrapValue = distance + speed;
	WrapMax = Spline->GetSplineLength();
	WrapMin = 0;

	// Wrap関数を使いdistanceを更新する
	distance = FMath::Wrap(WrapValue, WrapMin, WrapMax);

	// このスプラインの長さに沿った距離を与えると、スプライン上のその点に対応するFTransformを返します。（BPより）
	return Spline->GetTransformAtDistanceAlongSpline(distance, ESplineCoordinateSpace::World);
}

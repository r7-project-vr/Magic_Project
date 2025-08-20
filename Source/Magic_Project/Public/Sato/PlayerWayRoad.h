// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerWayRoad.generated.h"

class USplineComponent;
UCLASS()
class MAGIC_PROJECT_API APlayerWayRoad : public AActor
{
	GENERATED_BODY()
	
private:	
	// Sets default values for this actor's properties
	APlayerWayRoad();

	// 変数たち
	float WrapValue;
	float WrapMin;
	float WrapMax;

public:

	UPROPERTY(EditAnywhere)
	TObjectPtr<USplineComponent> Spline;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//distanceとspeedから、スプラインのトランスフォームを返す
	FTransform GetSplineTransform(float& distance, float speed);
};
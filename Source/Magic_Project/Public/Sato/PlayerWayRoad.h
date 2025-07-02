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
	
public:	
	// Sets default values for this actor's properties
	APlayerWayRoad();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//一個下のやつだけデバッグ用なので後で必ず消すこと！！！！！sato
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "SplineComponent")
	int abcdefghijklmn;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "SplineComponent")
	TObjectPtr<USplineComponent> Spline;
};

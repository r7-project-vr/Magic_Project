// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActorHan.h"

// Sets default values
AMyActorHan::AMyActorHan()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyActorHan::BeginPlay()
{
	Super::BeginPlay();
	//GetWorld()->MyActorHan<AActor>(PlayerActor, FTransform(FVector(0, 0, 0)));
	GetWorld()->SpawnActor<AActor>(PlayerActor, FTransform(FVector(0, 0, 0)));
}

// Called every frame
void AMyActorHan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


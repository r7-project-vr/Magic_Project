// Fill out your copyright notice in the Description page of Project Settings.


#include "KanndaTestChara.h"

// Sets default values
AKanndaTestChara::AKanndaTestChara()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AKanndaTestChara::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKanndaTestChara::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AKanndaTestChara::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


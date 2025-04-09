// Fill out your copyright notice in the Description page of Project Settings.


#include "KandaPawn.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
AKandaPawn::AKandaPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// StaticMeshComponent‚ğ’Ç‰Á‚µARootComponent‚Éİ’è‚·‚é
	Sphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = Sphere;

	// StaticMesh‚ğLaod‚µ‚ÄStaticMeshComponent‚ÌStaticMesh‚Éİ’è‚·‚é
	UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere"));

	// StaticMesh‚ğStaticMeshComponent‚Éİ’è‚·‚é
	Sphere->SetStaticMesh(Mesh);

	// Material‚ğStaticMesh‚Éİ’è‚·‚é
	UMaterial* Material = LoadObject<UMaterial>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial"));

	// Material‚ğStaticMeshComponent‚Éİ’è‚·‚é
	Sphere->SetMaterial(0, Material);

	// Simulate Physics‚ğ—LŒø‚É‚·‚é
	Sphere->SetSimulatePhysics(true);
}

// Called when the game starts or when spawned
void AKandaPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKandaPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AKandaPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


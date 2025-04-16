// Fill out your copyright notice in the Description page of Project Settings.


#include "Magic/Onishi_MagicLauncher.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"

// Sets default values
AOnishi_MagicLauncher::AOnishi_MagicLauncher()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOnishi_MagicLauncher::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle DestroyTimerHandle;
	GetWorldTimerManager().SetTimer(DestroyTimerHandle,this,&AOnishi_MagicLauncher::HandleAutoDestroy,2.0f,false);
}

// Called every frame
void AOnishi_MagicLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLocation = GetActorLocation() + (MoveDirection.GetSafeNormal() * MoveSpeed * DeltaTime);
	SetActorLocation(NewLocation);

	
}

void AOnishi_MagicLauncher::HandleAutoDestroy()
{
	// エフェクト再生（あれば）
	if (DestroyEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			DestroyEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	// アクターを削除
	Destroy();
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Magic/Onishi_MagicLauncher.h"
#include "Kismet/KismetSystemLibrary.h"
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
	
	//破壊までのタイマーを始める（3つ目の引数で指定（単位は秒））
	FTimerHandle DestroyTimerHandle;
	GetWorldTimerManager().SetTimer(DestroyTimerHandle,this,&AOnishi_MagicLauncher::HandleAutoDestroy,2.0f,false);

	//最初の位置を決定
	//SetActorLocation(StartLocation);

	UKismetSystemLibrary::PrintString(
		this,
		StartLocation.ToString(),
		true,
		true,
		FColor::Blue,
		5.0f
	);
}

// Called every frame
void AOnishi_MagicLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//自動で前に進む
	//FVector NewLocation = GetActorLocation() + (MoveDirection.GetSafeNormal() * MoveSpeed * DeltaTime);
	//SetActorLocation(NewLocation);
}

void AOnishi_MagicLauncher::HandleAutoDestroy()
{
	// エフェクト再生（あれば）
	//継承先のBPで指定できる（詳細のウィンドウで）
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

void AOnishi_MagicLauncher::LaunchMagic(FVector Facing, FVector NowLocation, FString EffectPath) {
	MoveDirection = Facing;
	StartLocation = NowLocation;
	DestroyEffect = LoadObject<UNiagaraSystem>(nullptr, *EffectPath);
}
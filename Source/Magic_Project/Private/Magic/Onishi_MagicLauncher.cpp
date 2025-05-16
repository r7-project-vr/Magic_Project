// Fill out your copyright notice in the Description page of Project Settings.


#include "Magic/Onishi_MagicLauncher.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"

// Sets default values
AOnishi_MagicLauncher::AOnishi_MagicLauncher()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 初期化
	DestroyEffect = nullptr;
	_NiagaraComponent = nullptr;

	// StaticMeshComponentを追加し、RootComponentに設定する
	Sphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = Sphere;
}

// Called when the game starts or when spawned
void AOnishi_MagicLauncher::BeginPlay()
{
	Super::BeginPlay();

#if false
	// 追記_5_16
	{
		//破壊までのタイマーを始める（3つ目の引数で指定（単位は秒））
		FTimerHandle DestroyTimerHandle;
		GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AOnishi_MagicLauncher::HandleAutoDestroy, 2.0f, false);

		//最初の位置を決定
		SetActorLocation(StartLocation);

		//飛ぶエフェクトの再生
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			FlyingEffect,
			GetActorLocation(),
			FRotator(0, -90, 0)
		);
	}
#endif
}

// Called every frame
void AOnishi_MagicLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//自動で前に進む
	FVector NewLocation = GetActorLocation() + (MoveDirection.GetSafeNormal() * MoveSpeed * DeltaTime);
	SetActorLocation(NewLocation);

	// RootComponentに併せて飛ばす
	MoveMagic();
}

void AOnishi_MagicLauncher::LaunchMagic(FVector Facing, FVector NowLocation, FString EffectPath) {
	MoveDirection = Facing;
	StartLocation = NowLocation;
	DestroyEffect = LoadObject<UNiagaraSystem>(nullptr, *EffectPath);

	// 追記_5_16
	{
		CreateMagicEffect(EffectPath);

		//破壊までのタイマーを始める（3つ目の引数で指定（単位は秒））
		FTimerHandle DestroyTimerHandle;
		GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AOnishi_MagicLauncher::HandleAutoDestroy, 2.0f, false);
	}
}

void AOnishi_MagicLauncher::CreateMagicEffect(FString EffectPath) {

	// ファイルが指定されてなければ処理なし
	if (*EffectPath == nullptr) { return; }

	UNiagaraSystem* ns = LoadObject<UNiagaraSystem>(nullptr, *EffectPath);

	// コンポーネントとしてNiagaraComponentを追加
	_NiagaraComponent = NewObject<UNiagaraComponent>(this);
	// コンポーネントにNiagaraSystemをセットして、シーンに追加
	_NiagaraComponent->SetAsset(ns);
	_NiagaraComponent->RegisterComponent();

	// 位置などの設定があれば、Transformを設定
	FRotator rotation = GetActorRotation();
	FVector location = GetActorLocation();

	_NiagaraComponent->SetWorldLocation(location);
	_NiagaraComponent->SetWorldRotation(rotation);

	// エフェクトの再生
	_NiagaraComponent->Activate();
}

void AOnishi_MagicLauncher::HandleAutoDestroy()
{
	//// エフェクト再生（あれば）
	////継承先のBPで指定できる（詳細のウィンドウで）
	//if (DestroyEffect)
	//{
	//	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
	//		GetWorld(),
	//		DestroyEffect,
	//		GetActorLocation(),
	//		MoveDirection.Rotation()
	//	);
	//}

	//DebugLogLocation(this, FColor::Black);

	// アクターを削除
	Destroy();
}

void AOnishi_MagicLauncher::MoveMagic(){

	if (_NiagaraComponent == nullptr) { return; }

	// 位置などの設定があれば、Transformを設定
	FRotator rotation = GetActorRotation();
	FVector location = GetActorLocation();

	_NiagaraComponent->SetWorldLocation(location);
	_NiagaraComponent->SetWorldRotation(rotation + FRotator(0, -90, 0));

}

void AOnishi_MagicLauncher::DebugLogLocation(AActor* a_, FColor c)
{
	if (a_ == nullptr) { return; }

	UKismetSystemLibrary::PrintString(
		this,
		a_->GetActorLocation().ToString(),
		true,
		true,
		c,
		2.0f
	);
}
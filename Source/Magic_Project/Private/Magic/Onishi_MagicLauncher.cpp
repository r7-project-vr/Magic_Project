// Fill out your copyright notice in the Description page of Project Settings.


#include "Magic/Onishi_MagicLauncher.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "NiagaraFunctionLibrary.h"
#include "Magic/Onishi_MagicCircleParent.h"
#include "Kanda/VRActor_ver1.h"

// Sets default values
AOnishi_MagicLauncher::AOnishi_MagicLauncher()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 初期化
	DestroyEffect = nullptr;
	_NiagaraComponent = nullptr;

	// StaticMeshComponentを追加し、RootComponentに設定する
	Magic = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = Magic;

	//スフィアコリジョン作成
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetCollisionProfileName("OverlapAll");
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetSphereRadius(50.0f);

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AOnishi_MagicLauncher::OnHit);


	SphereComponent->SetHiddenInGame(false);
	SphereComponent->SetVisibility(true);
}

// Called when the game starts or when spawned
void AOnishi_MagicLauncher::BeginPlay()
{
	Super::BeginPlay();

	{
		FVector PreloadDirection = -GetActorForwardVector();
		FVector PreloadLocation = GetActorLocation();
		UNiagaraSystem* PreloadFlyingEffect = FlyingEffect;
		UNiagaraSystem* PreloadDestroyEffect = DestroyEffect;

		LaunchMagic(PreloadDirection, PreloadLocation, PreloadFlyingEffect, PreloadDestroyEffect);
	}
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

void AOnishi_MagicLauncher::LaunchMagic(FVector Facing, FVector NowLocation, UNiagaraSystem* Ef_Flying, UNiagaraSystem* Ef_Destroy) {
	MoveDirection = Facing;
	StartLocation = NowLocation;
	DestroyEffect = Ef_Destroy;

	// 追記_5_16
	{
		CreateMagicEffect(Ef_Flying);

		//破壊までのタイマーを始める（3つ目の引数で指定（単位は秒））
		FTimerHandle DestroyTimerHandle;
		GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AOnishi_MagicLauncher::HandleAutoDestroy, 2.0f, false);
	}
}

void AOnishi_MagicLauncher::CreateMagicEffect(UNiagaraSystem* Effect) {

	// ファイルが指定されてなければ処理なし
	if (Effect == nullptr) { return; }

	UNiagaraSystem* ns = Effect;

	// コンポーネントとしてNiagaraComponentを追加
	_NiagaraComponent = NewObject<UNiagaraComponent>(this);
	// コンポーネントにNiagaraSystemをセットして、シーンに追加
	_NiagaraComponent->SetAsset(ns);
	_NiagaraComponent->RegisterComponent();

	// 位置などの設定があれば、Transformを設定
	FRotator rotation = GetActorRotation();
	FVector location = GetActorLocation();

	_NiagaraComponent->SetWorldLocation(location);
	_NiagaraComponent->SetWorldRotation(rotation + FRotator(0, -90, 0));

	_NiagaraComponent->SetupAttachment(RootComponent);

	// エフェクトの再生
	_NiagaraComponent->Activate();
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
			MoveDirection.Rotation()
		);
	}

	DebugLogLocation(this, FColor::Blue);

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

	/*UKismetSystemLibrary::PrintString(
		this,
		a_->GetActorLocation().ToString(),
		true,
		true,
		c,
		2.0f
	);*/
}

void AOnishi_MagicLauncher::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult) {

	if (OtherActor->IsA(AVRActor_ver1::StaticClass())) {
		return;
	}
	else if (OtherActor->IsA(AOnishi_MagicCircleParent::StaticClass())) {
		//UKismetSystemLibrary::PrintString(
		//	this,
		//	TEXT("MagicCircleHit"),
		//	true,
		//	true,
		//	FColor::Blue,
		//	2.0f
		//);
		return;
	}
	else {
			HandleAutoDestroy();
			if (CollisionEffect)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					GetWorld(),
					CollisionEffect,
					GetActorLocation(),
					MoveDirection.Rotation()
				);
			}
			//UKismetSystemLibrary::PrintString(
			//	this,
			//	TEXT("HIT"),
			//	true,
			//	true,
			//	FColor::Red,
			//	2.0f
			//);
	}
}
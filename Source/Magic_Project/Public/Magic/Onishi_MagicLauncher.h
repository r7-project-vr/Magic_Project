// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"// 追記
#include "Components/StaticMeshComponent.h"
#include "Onishi_MagicLauncher.generated.h"

UCLASS()
class MAGIC_PROJECT_API AOnishi_MagicLauncher : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOnishi_MagicLauncher();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed = 100.0f; // cm/s

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	/*FVector MoveDirection = FVector(1.0, 0.0, 0.0);*/
	//書き換えた 5_16
	FVector MoveDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* DestroyEffect;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Movement")
	/*FVector StartLocation = FVector(0, 0, 0);*/ 
	//書き換えた 5_16
	FVector StartLocation;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Effects")
	UNiagaraSystem* FlyingEffect;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* CollisionEffect;

	//スフィアコリジョン
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* SphereComponent;

	// 追記
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraComponent* _NiagaraComponent;

	UPROPERTY(VisibleAnywhere, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Magic;
public:
	UFUNCTION()
	void HandleAutoDestroy();

	///<summary>
	///魔法を発射するときに呼び出し
	///向き/現在位置/魔法のエフェクトを指定
	///</summary>
	UFUNCTION()
	void LaunchMagic(FVector Facing, FVector NowLocation, UNiagaraSystem* Ef_Flying, UNiagaraSystem* Ef_Destroy);

private:

	// ----------------------
	// 追記_5_16

	// プレイヤーから生成される魔法のエフェクト
	void CreateMagicEffect(UNiagaraSystem* Effect);

	void MoveMagic();

	// デバッグ
	void DebugLogLocation(AActor* a_, FColor c);

	// 魔法が壁に衝突したときの処理
	UFUNCTION()
	void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
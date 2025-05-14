// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
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
	float MoveSpeed = 200.0f; // cm/s

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector MoveDirection = FVector(1.0f, 0.0f, 0.0f); // 仮指定X方向

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* DestroyEffect;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Movement")
	FVector StartLocation = FVector(0, 0, 0);

public:
	UFUNCTION()
	void HandleAutoDestroy();

	//魔法を発射するときに呼び出し
	//向き/現在位置/魔法のエフェクト(path)を指定
	UFUNCTION()
	void LaunchMagic(FVector Facing, FVector NowLocation, FString EffectPath);
};

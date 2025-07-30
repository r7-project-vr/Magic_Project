// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Onishi_MagicCircleParent.generated.h"


UCLASS()
class MAGIC_PROJECT_API AOnishi_MagicCircleParent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOnishi_MagicCircleParent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//スフィアコリジョン
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* SphereComponent;

	//オーバーラップした際の処理を書く関数
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	//オーバーラップから抜けた際の処理を書く関数
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	// 魔法の射撃回数
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Effects")
	int magicCnt;

	//魔法陣としてのエフェクト
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* Ef_MagicCircle;

	//オーバーラップしたプレイヤーのアクターを保存
	UPROPERTY()
	AActor* OverlappingActor;

	//飛ばす魔法のエフェクト
	UPROPERTY(EditAnywhere,BluePrintReadWrite,Category="Effects")
	TArray<UNiagaraSystem*> Ef_MagicFly;

	//魔法が飛んだ後最後に出るエフェクト
	UPROPERTY(EditAnywhere,BluePrintReadWrite,Category="Effects")
	TArray<UNiagaraSystem*> Ef_Destroy; 

	//魔方陣入る
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic Circle Status")
	bool bIsPlayerOverlapping;
};

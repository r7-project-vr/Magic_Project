// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "HitObjectParent.generated.h"

class UStaticMeshComponent;

UCLASS()
class MAGIC_PROJECT_API AHitObjectParent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHitObjectParent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "ObjectMesh")
	TObjectPtr<UStaticMeshComponent> ObjectMesh;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Material")
	UMaterial* StartMaterial;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Material")
	TArray<UMaterial*> ChangeMaterial;

protected:

	// オーバーラップした際の処理を書く関数
	UFUNCTION(BlueprintCallable)
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	// 接触判定のコリジョン
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* SphereComponent;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "InputActionValue.h"
#include "Magic/Onishi_MagicCircleParent.h"
#include "Kanda/MagicDataTable.h"
#include "VRActor_ver1.generated.h"

class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UArrowComponent;
class UInputMappingContext;
class UInputAction;
class USphereComponent;

UCLASS()
class MAGIC_PROJECT_API AVRActor_ver1 : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVRActor_ver1();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	/** Character用のStaticMesh : Sphere */
	UPROPERTY(VisibleAnywhere, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Player;

	UPROPERTY(VisibleAnywhere, Category = Collider, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	// コントローラーのマッピング
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ControlMove;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MagicCharge;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ControlMagic;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	//スプラインアクター格納用
public:
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "SplineActor")
	TArray<AActor*> SplineActor;

public:
	UFUNCTION()
	void CreateMagic(UNiagaraSystem* Ef_Flying_, UNiagaraSystem* Ef_Destroy_, float MagicSpeed = 10.f);

	void SetMagicData(TSharedPtr<MagicDataTable> m_, AOnishi_MagicCircleParent* o_);

private:

	// 接触判定の処理、コライダー同士が接触したときに呼び出される
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 接触判定の処理、コライダー同士が離れたときに呼び出される
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	
	//playerコントロール
	void ControlPlayer(const FInputActionValue& Value);

	// 魔法のチャージ
	void ChargeMagic();

	//魔法コントロール
	void GoMagic(const FInputActionValue& Value);

	// カメラコントロール
	void Look(const FInputActionValue& Value);

private:

	// 移動倍率
	float MoveSpeedPoint = 10.0f;

	// 移動方向
	FRotator MoveRotator;

	// 魔法のエフェクトのファイルを登録
	FString MagicEffectFilePath[9];

	void DebugLogLocation(AActor* a_ , FColor c);

	// 魔法のデータ管理用
	TSharedPtr<MagicDataTable> magicData;

	// 魔法陣のポインタ
	AOnishi_MagicCircleParent* circle;

	//魔法をためた時間を計測
	float MagicChargeTime = 0.0f;

	//VR機器の情報
	void VRInformation();

	//スプラインのTransformを取得する関数
	void GetSplineTransform(float& distance, float speed);

	//----------------------------------------
	// csv用
	//----------------------------------------
	FString MagicFilePath;

	// csvファイル出力
	void WritePlayerInfoToCSV(AActor* m_);

public:
	// 魔法実行フラグ
	bool CanMagic = true;//とりあえず

	void kari();
};

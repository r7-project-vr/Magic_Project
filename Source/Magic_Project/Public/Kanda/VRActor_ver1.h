// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "InputActionValue.h"
#include "Magic/Onishi_MagicCircleParent.h"
#include "Kanda/MagicDataTable.h"
#include "sato/PlayerWayRoad.h"
#include "ASerialCom/Public/ASerialLibControllerWin.h"
#include "ASerialCom/Public/ASerialCore/ASerialPacket.h"
#include "sato/MagicDeviceCmdSender.h"
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

	// Finalize相当の関数
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	/** Character用のStaticMesh : Sphere */
	UPROPERTY(VisibleAnywhere, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Player;

	UPROPERTY(VisibleAnywhere, Category = Collider, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
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

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccses = "true"))
	TObjectPtr<UInputAction> MoveStart;

protected:
	//スプラインアクター格納用
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineActorHere")
	TObjectPtr<APlayerWayRoad> SplineActor;

private:
	// デバイス用
	UPROPERTY()
	UASerialLibControllerWin* device_;

	UPROPERTY()
	UASerialPacket* deviceInfo_;

	UPROPERTY()
	AMagicDeviceCmdSender* deviceCmd_;

	// テンプレート関数
	template<typename T>
	T TransformDataToInt(const uint8_t* Data, int Size) const;


	// 通信処理速度制限用の変数
	// Interval = 1.0f / xx.xf;で何fpsか制限できる
	float TimeAccumulator = 0.0f;
	const float Interval = 1.0f / 60.0f; 

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
	void ChargeMagic(const FInputActionValue& Value);

	//魔法コントロール
	void GoMagic(const FInputActionValue& Value);

	// カメラコントロール
	void Look(const FInputActionValue& Value);

	// プレイヤーの移動開始
	void PlayerMoveStart(const FInputActionValue& Value);

private:

	// 移動倍率
	float MoveSpeedPoint = 30.0f;

	// 移動方向
	FRotator MoveRotator;

	// スプライン用変数
	float distance;

	// いま移動できるかどうか。trueで停止中。
	bool isStop = true;

	// スプライン上の点で止まるために番号を指定する変数（現在は自動で指定）
	int StopPointNum = 1;

	void DebugLogLocation(AActor* a_ , FColor c);

	// 魔法のデータ管理用
	TSharedPtr<MagicDataTable> magicData;

	// 魔法陣のポインタ
	AOnishi_MagicCircleParent* circle;

	// 魔法をためた時間を計測
	float MagicChargeTime = 0.0f;

	// VR機器の情報
	void VRInformation();

	// スプラインの指定した点に着いたら行う処理
	void ArriveSplinePoint(int point_);
	
	//----------------------------------------
	// csv用
	//----------------------------------------
	FString MagicFilePath;

	// csvファイル出力
	void WritePlayerInfoToCSV(AActor* m_);

public:
	// 魔法実行フラグ
	
	/// <summary>
	/// とりあえず
	/// </summary>
	bool CanMagic = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Settings")
	bool IsInMagicZone = false;

	//魔方陣入る
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Magic")
	bool bIsPlayerOverlapping = false;

};

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
#include "Sound/SoundBase.h"
#include "NiagaraSystem.h"
#include "Sato/DeviceThreadManager.h"
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

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UArrowComponent> Arrow;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Sphere_HandTest;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraComponent> ChargingEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraComponent> ChargeFinishEffect;

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

	// 効果音
	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* NormalMagicSound;

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* ChargeMagicSound;

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* ChargeFinishSound;

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* HandUpSound;

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* HandDownSound;

	// エフェクト
	UPROPERTY(EditAnywhere, Category = Niagara)
	UNiagaraSystem* HandStar;

	UPROPERTY(EditAnywhere, Category = Niagara)
	UNiagaraSystem* ChargingMagicEffect;
	
	UPROPERTY(EditAnywhere, Category = Niagara)
	UNiagaraSystem* ChargeFinishMagicEffect;

protected:
	//スプラインアクター格納用
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineActorHere")
	TObjectPtr<APlayerWayRoad> SplineActor;

private:
	// デバイス用
	//UPROPERTY()
	//UASerialLibControllerWin* device_;

	//UPROPERTY()
	//UASerialPacket* deviceInfo_;

	//UPROPERTY()
	//AMagicDeviceCmdSender* deviceCmd_;

	UPROPERTY()
	UDeviceThreadManager* DeviceManager_;

	UFUNCTION()
	void DeviceRotateToAverage();

	// デバイスの情報を入れるとオイラー角を取得できる関数
	// TransformDataToInt32はTransformEulerAnglesのために作られた関数です。
	// 使う際はTransformEulerAnglesにデバイスの情報を入れればそのまま使えます。
	int32 TransformDataToInt32(const uint8_t* Data, int Size);
	FRotator TransformEulerAngles(const uint8_t* Data, int Size);


	// 通信処理速度制限用の変数
	// Interval = 1.0f / xx.xf;で何fpsか制限できる
	float TimeAccumulator = 0.0f;
	const float Interval = 1.0f / 30.0f;

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
	void MouseChargeMagic(const FInputActionValue& Value);

	//魔法コントロール
	void GoMagic();

	// カメラコントロール
	void Look(const FInputActionValue& Value);

	// プレイヤーの移動開始
	//void kariPlayerMoveStart(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void PlayerMoveStart();

public:

	// スコア
	UPROPERTY(BlueprintReadWrite)
	int Magic_Score;

private:
	// 移動倍率
	float MoveSpeedPoint = 30.0f;

	// 移動方向
	FRotator MoveRotator;

	// スプライン用変数
	float distance;

	// いま移動できるかどうか。trueで停止中。
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool isStop = true;


	// [難易度管理]腕をどれだけ上げる必要があるか
	float ArmUpAngle;

	// 腕を上げ下げした回数
	int ArmUpDownCnt = 0;

	// [難易度管理]移動に必要な腕を上げ下げする回数
	int Need_ArmUpDownCnt;

	// 今腕を上げているか下げているか。trueで上げている。
	bool IsArmUp = false;

	// 魔法のチャージ中のエフェクト用の変数
	bool alreadyChargingMagicEffect = false;
	bool alreadyChargeFinishMagicEffect = false;

	//
	int count;
	float Average;

	// デバイスからもらった、今どれだけの角度を向いているかを表す変数
	FRotator Final_Device_Rotate;

	// Final_Device_Rotateの前身である変数。平均をとるため
	FRotator AverageRotate;

	// スプライン上の点で止まるために番号を指定する変数（現在は自動で指定）
	int StopPointNum = 1;

	void DebugLogLocation(AActor* a_, FColor c);

	// 魔法のデータ管理用
	TSharedPtr<MagicDataTable> magicData;

	// 魔法陣のポインタ
	AOnishi_MagicCircleParent* circle;

	// 魔法をためた時間を計測
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float MagicChargeTime = 0.0f;

	// 魔法をチャージしてるときにエフェクトを出す関数
	void SpawnMagicChargeEffect();

	// VR機器の情報
	void VRInformation();

	// スプラインの指定した点に着いたら行う処理
	void ArriveSplinePoint(int point_);

	// デバイスで魔法を放つ処理
	void DeviceGoMagic();

	void ResetCharged();

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Charged")
	bool Charged = false;
};

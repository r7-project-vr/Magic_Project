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
//#include "ASerialCom/Public/ASerialLibControllerWin.h"
//#include "ASerialCom/Public/ASerialCore/ASerialPacket.h"
#include "sato/MagicDeviceCmdSender.h"
#include "Sound/SoundBase.h"
#include "NiagaraSystem.h"
#include "Sato/DeviceThreadManager.h"
#include "sato/WirelessDeviceManager.h"
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

	/// <summary>
	/// スタティックメッシュコンポーネント
	/// </summary>
	UPROPERTY(VisibleAnywhere, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Player;

	/// <summary>
	/// スフィアコンポーネント
	/// </summary>
	UPROPERTY(VisibleAnywhere, Category = Collider, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> Sphere;

	/// <summary>
	/// カメラコンポーネント
	/// </summary>
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	/// <summary>
	/// アローコンポーネント
	/// </summary>
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UArrowComponent> Arrow;

	/// <summary>
	/// スタティックメッシュコンポーネント
	/// 現実と連動するゲーム内の手
	/// </summary>
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> InGameHand;

	/// <summary>
	/// スプリングアームコンポーネント
	/// </summary>
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	/// <summary>
	/// 魔法チャージ中のエフェクト
	/// </summary>
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraComponent> ChargingEffect;

	/// <summary>
	/// 魔法チャージが完了した後のエフェクト
	/// </summary>
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraComponent> ChargeFinishEffect;

	//==================================================
	//コントローラー
	//==================================================

	/// <summary>
	/// コントローラーのマッピング
	/// </summary>
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	/// <summary>
	/// デバッグ用
	/// 移動アクション
	/// </summary>
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ControlMove;

	/// <summary>
	/// キャリブレーション開始アクション
	/// </summary>
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> CalibrationStart;

	/// <summary>
	/// 魔法チャージアクション
	/// </summary>
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MagicCharge;

	/// <summary>
	/// 魔法発射アクション
	/// </summary>
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ShotMagic;

	/// <summary>
	/// デバッグ用
	/// カメラアクション
	/// </summary>
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	/// <summary>
	/// デバッグ用
	/// 移動開始ボタンアクション
	/// </summary>
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccses = "true"))
	TObjectPtr<UInputAction> MoveStart;


	//==================================================
	//サウンド
	//==================================================

	/// <summary>
	/// 通常魔法サウンド
	/// </summary>
	UPROPERTY(EditAnywhere, Category = Sound)
	TObjectPtr<USoundBase> NormalMagicSound;

	/// <summary>
	/// チャージ魔法サウンド
	/// </summary>
	UPROPERTY(EditAnywhere, Category = Sound)
	TObjectPtr<USoundBase> ChargeMagicSound;

	/// <summary>
	/// 魔法チャージ完了サウンド
	/// </summary>
	UPROPERTY(EditAnywhere, Category = Sound)
	TObjectPtr<USoundBase> ChargeFinishSound;

	/// <summary>
	/// 手が閾値まで上がった際のサウンド
	/// </summary>
	UPROPERTY(EditAnywhere, Category = Sound)
	TObjectPtr<USoundBase> HandUpSound;

	/// <summary>
	/// 手が閾値まで下がった際のサウンド
	/// </summary>
	UPROPERTY(EditAnywhere, Category = Sound)
	TObjectPtr<USoundBase> HandDownSound;

	//==================================================
	//エフェクト
	//==================================================

	/// <summary>
	/// 手が閾値まで到達した際に出るエフェクト
	/// </summary>
	UPROPERTY(EditAnywhere, Category = Niagara)
	TObjectPtr<UNiagaraSystem> HandStar;

	/// <summary>
	/// 魔法チャージ中のエフェクト
	/// </summary>
	UPROPERTY(EditAnywhere, Category = Niagara)
	UNiagaraSystem* ChargingMagicEffect;

	/// <summary>
	/// 魔法チャージ完了したエフェクト
	/// </summary>
	UPROPERTY(EditAnywhere, Category = Niagara)
	UNiagaraSystem* ChargeFinishMagicEffect;

protected:
	//スプラインアクター格納用
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineActorHere")
	TObjectPtr<APlayerWayRoad> SplineActor;

private:
	/// <summary>
	/// デバイス
	/// </summary>
	UPROPERTY()
	UDeviceThreadManager* DeviceManager_;

	// デバイスの情報を入れるとオイラー角を取得できる関数
	// TransformDataToInt32はTransformEulerAnglesのために作られた関数です。
	// 使う際はTransformEulerAnglesにデバイスの情報を入れればそのまま使えます。
	int32 TransformDataToInt32(const uint8_t* Data, int Size);
	FRotator TransformEulerAngles(const uint8_t* Data, int Size);

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

	// キャリブレーション開始
	void CalibrationInitialize(const FInputActionValue& Value);

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


	// 腕をどれだけ上げる必要があるか(キャリブレーション済み)
	float ArmUpAngle;

	UPROPERTY()
	bool bIsCalibrationStart = false;

	// チュートリアルのキャリブレーションに必要な腕の上下回数
	UPROPERTY()
	int Need_Calibration_ArmUpDownCnt = 3;

	// チュートリアルのキャリブレーションに必要な腕の角度
	UPROPERTY()
	float Need_Calibration_ArmUpAngle = -70.0f;

	// キャリブレーションで取った腕の最高到達点
	UPROPERTY()
	int Calibration_ArmUpMaxAngle[3] = { -70,-70,-70 };

	/// <summary>
	/// キャリブレーションのために腕を上げ下げした回数。キャリブレーションの処理が終わるまではこの回数を数える
	/// </summary>
	UPROPERTY()
	int CalibrationCnt = 0;

	/// <summary>
	/// キャリブレーションのために腕を上げているかどうか
	/// </summary>
	UPROPERTY()
	bool bIsArmUpCalibration = false;

	/// <summary>
	/// キャリブレーションの合計角度を保持する
	/// </summary>
	UPROPERTY()
	float CalibrationTotalAngle = 0.f;

	/// <summary>
	/// キャリブレーションの計算が終わったかどうか
	/// </summary>
	UPROPERTY()
	bool bIsCalibrated;

	/// <summary>
	/// チュートリアルの最初に行うキャリブレーションの処理
	/// </summary>
	UFUNCTION()
	void FirstCalibration();

	// 腕を上げ下げした回数
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int ArmUpDownCnt = 0;

	bool AlreadyMove = false;

	// [難易度管理]移動に必要な腕を上げ下げする回数
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int Need_ArmUpDownCnt;

	// 今腕を上げているか下げているか。trueで上げている。
	bool IsArmUp = false;

	// 魔法のチャージ中のエフェクト用の変数
	bool alreadyChargingMagicEffect = false;
	bool alreadyChargeFinishMagicEffect = false;

	/// <summary>
	/// 無線通信のデバイス
	/// </summary>
	UPROPERTY()
	TObjectPtr<UWirelessDeviceManager> WirelessDevice;

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

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ASerialLibControllerWin.h"
#include "HAL/Runnable.h"
#include "HAL/ThreadSafeCounter.h"
#include "ASerialCom/Public/ASerialLibControllerWin.h"
#include "ASerialCom/Public/ASerialCore/ASerialPacket.h"
#include "Templates/SharedPointer.h"


/**
 *
 */
class MAGIC_PROJECT_API FMagicDeviceCmdSender : public FRunnable
{	
public:	
	FMagicDeviceCmdSender();
	~FMagicDeviceCmdSender();

	// FRunnableインターフェースの実装
	virtual bool Init() override;          // スレッド開始時に呼ばれる初期化処理
	virtual uint32 Run() override;         // メイン処理（スレッドの中身）
	virtual void Stop() override;          // 停止要求時に呼ばれる
	virtual void Exit() override;          // スレッド終了直前に呼ばれる（任意）

private:
	FThreadSafeCounter StopTaskCounter;    // スレッド停止フラグとして使う

	// デバイス用変数
	UPROPERTY()
	UASerialLibControllerWin* device_;

	UPROPERTY()
	UASerialPacket* deviceInfo_;

	//TSharedPtr<FMagicDeviceCmdSender> deviceCmd_;
	//FMagicDeviceCmdSender* deviceCmd_;


	// デバイスの情報を入れるとオイラー角を取得できる関数
	// TransformDataToInt32はTransformEulerAnglesのために作られた関数です。
	// 使う際はTransformEulerAnglesにデバイスの情報を入れればそのまま使えます。
	// あとでサマリーにするsato
	int32 TransformDataToInt32(const uint8_t* Data, int Size);
	FRotator TransformEulerAngles(const uint8_t* Data, int Size);

	
public:
	/// <summary>
	/// デバイスの情報をReadDataしたもの（未変換）
	/// publicなのあとでどうしよう。DeviceThreadManagerのためパブリックにしましたsato
	/// このへんぐちゃぐちゃ
	/// </summary>
	FRotator DeviceRotate_;

public:
	FCriticalSection DataMutex;

public:
	/// <summary>
	/// キャリブレーションコマンドを送信する関数
	/// </summary>
	/// <param name="device_">デバイス</param>
	void SendCmd_Cali(UASerialLibControllerWin* magicdevice_);

	/// <summary>
	/// オイラー角取得コマンドを送信する関数
	/// </summary>
	/// <param name="device_">デバイス</param>
	/// <returns>デバイス情報のオイラー角</returns>
	int32 SendCmd_Euler(UASerialLibControllerWin* magicdevice_);

	/// <summary>
	/// クォータニオン取得コマンドを送信する関数
	/// </summary>
	/// <param name="device_">デバイス</param>
	/// <returns>デバイス情報のクォータニオン</returns>
	int32 SendCmd_Quater(UASerialLibControllerWin* magicdevice_);

private:
	/// <summary>
	/// デバイスのID
	/// </summary>
	const uint8_t DeviceID_ = 0x02;
	
	/// <summary>
	/// デバイスのバージョン
	/// </summary>
	const uint8_t DeviceVersion_ = 0x01;
private:
	/// <summary>
	/// キャリブレーション取得コマンド
	/// </summary>
	uint8_t Calibration_ = 0x20;

	/// <summary>
	/// オイラー角取得コマンド
	/// </summary>
	uint8_t EulerCmd_ = 0x21;

	/// <summary>
	/// クォータニオン取得コマンド
	/// </summary>
	uint8_t QuaternionCmd_ = 0x22;
};

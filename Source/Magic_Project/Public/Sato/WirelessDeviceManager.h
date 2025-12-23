// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "sato/MahonyAHRS.h"
#if PLATFORM_ANDROID
#include "Interface/BleManagerInterface.h"
#endif
#include "WirelessDeviceManager.generated.h"

//===================================================
// BLE IO_SERVICE
//===================================================
#define IO_DEVICE_NAME "MagicTracker"
//98:A3:16611B62

// IMUService UUID
#define IO_IMUSERVICE_UUID "164a9bb4-e44e-f183-3f22-4fa4ff1f2338"
// Read Characteristic UUID
#define IO_ROTATEINFO_CHARACTERISTIC_UUID "fb4db371-b721-eeff-8bde-5a8c7c100ab0"
#define IO_QUATERNION_CHARACTERISTIC_UUID "f7e242cb-a41d-f609-bba1-9c0d59a78267"
#define IO_EULER_CHARACTERISTIC_UUID "7fda4d20-38b5-3c23-389b-9d3a9ef61c65"
// Write And ReadCharacteristic UUID
#define IO_CALIBRATION_CHARACTERISTIC_UUID "d45532fa-d9cb-4d53-88a8-1be5e5232dc1"

// BatteryService UUID
#define IO_BATTERYSERVICE_UUID "0x180F"
// Read Characteristic UUID
#define IO_BATTERY_CHARACTERISTIC_UUID "0x2A19"
#define IO_ISCHARGING_CHARACTERISTIC_UUID "0c48713b-0134-3076-9dd9-b2e7c78b748d"

// PairingService UUID
#define IO_PAIRINGSERVICE_UUID "52f93fe3-013b-4256-8d87-fdcea414b533"
// Read Characteristic UUID 
#define IO_ISPUSHBUTTON_CHARACTERISTIC_UUID "df59a71f-82c2-0f07-1fdf-f7777c822823"
// Write Charateristic UUID
#define IO_PAIRINGREQUEST_CHARACTERISTIC_UUID "abaae5f2-4829-121c-1595-0e80c6f0e78a"


// Android Permission
#define ANDROID_FILE_LOCATION_PERMISSION "android.permission.ACCESS_FINE_LOCATION"
#define ANDROID_BLUETOOTH_CONNECT_PERMISSION "android.permission.BLUETOOTH_CONNECT"
#define ANDROID_BLUETOOTH_SCAN_PERMISSION "android.permission.BLUETOOTH_SCAN"


class IBleManagerInterface;

/// <summary>
/// デバイスの接続状況
/// </summary>
UENUM(BlueprintType)
enum class EDeviceConnectType : uint8
{
	Connecting,
	Connected,
	Disconnecting,
	UnConnected
};

/// <summary>
/// IMUデータ構造体
/// </summary>
USTRUCT(BlueprintType)
struct FIMUData {
	GENERATED_BODY()
	int16_t accel_x;
	int16_t accel_y;
	int16_t accel_z;
	int16_t gyro_x;
	int16_t gyro_y;
	int16_t gyro_z;
};

/// <summary>
/// IMUオイラー角構造体
/// </summary>
USTRUCT(BlueprintType)
struct FIMUEulerAngles {
	GENERATED_BODY()
	float roll;
	float pitch;
	float yaw;
};


/**
 * デバイスの無線接続などを管理するクラス
 */
UCLASS()
class MAGIC_PROJECT_API UWirelessDeviceManager : public UObject
{
	GENERATED_BODY()

public:

	UWirelessDeviceManager();
	~UWirelessDeviceManager();

	void Init();
	bool Connect();
	bool Disconnect();

private:
	/// <summary>
	/// マホニークラス
	/// Initでインスタンス化すること
	/// </summary>
	//TObjectPtr<Mahony> mahony;
	
	/// <summary>
	/// Bluetooth周りの検査
	/// </summary>
	/// <returns></returns>
	bool CheckBluetooth();

	/// <summary>
	/// アンドロイドのbluetoothの権限を要求する
	/// </summary>
	void RequestAndroidPermission();

	/// <summary>
	/// 権限の要求結果
	/// </summary>
	/// <param name="Permissions"></param>
	/// <param name="GrantResults"></param>
	UFUNCTION()
	void OnPermissionResult(const TArray<FString>& Permissions, const TArray<bool>& GrantResults);

	/// <summary>
	/// 目標のサービスを決める
	/// </summary>
	void DecideTargetServices();

	/// <summary>
	/// 周囲のデバイスを探す
	/// </summary>
	void FindDeviceByServices();

	/// <summary>
	/// 新しいデバイスが見つかった時
	/// </summary>
	/// <param name="Device">デバイス</param>
	UFUNCTION()
	void OnDeviceFound(TScriptInterface<class IBleDeviceInterface> Device);

	/// <summary>
	/// コネクションが成功した時
	/// </summary>
	UFUNCTION()
	void OnConnectSucc();

	/// <summary>
	/// コネクションが失敗した時
	/// </summary>
	/// <param name="ErrorMessage">エラーメッセージ</param>
	UFUNCTION()
	void OnConnectError(FString ErrorMessage);

	/// <summary>
	/// 切断成功した時
	/// </summary>
	UFUNCTION()
	void OnDisconnectSucc();

	/// <summary>
	/// 切断失敗した時
	/// </summary>
	/// <param name="ErrorMessage">エラーメッセージ</param>
	UFUNCTION()
	void OnDisconnectError(FString ErrorMessage);

	/// <summary>
	/// 貰ったデータから必要な数値に変換
	/// </summary>
	/// <param name="Data">データ</param>
	/// <param name="Size">データサイズ</param>
	/// <returns>数値</returns>
	template<typename T>
	T TransformDataToInt32(const uint8_t* Data, int Size) const;

	/// <summary>
	/// 通知が来た時
	/// </summary>
	/// <param name="ServiceUUID">サービスUUID</param>
	/// <param name="CharacteristicUUID"></param>
	/// <param name="Data">データ</param>
	UFUNCTION()
	void OnReceiveData(FString ServiceUUID, FString CharacteristicUUID, TArray<uint8>& Data);

	/// <summary>
	/// 回転データを処理
	/// </summary>
	/// <param name="Data">データ/param>
	void HandleRotateData(TArray<uint8>& Data);

	/// <summary>
	/// バッテリーデータを処理
	/// </summary>
	/// <param name="Data">データ</param>
	void HandleBatteryData(const FIMUData& Data);

	/// <summary>
	/// もらったデータを表示する(デバッグ用)
	/// </summary>
	/// <param name="Data">データ</param>
	void DebugReceiveData(const FIMUData& Data);

	/// <summary>
	/// 生データを物理値に変換する
	/// </summary>
	/// <param name="Data">生データ</param>
	void RawDataConvertPhysicalValue(const FIMUData& Data);

	/// <summary>
	/// デバイスのデータをオイラー角に変換する関数
	/// </summary>
	/// <param name="Data">デバイスのデータ</param>
	/// <param name="Size">配列のサイズ</param>
	/// <returns>計算済みの回転量</returns>
	FRotator TransformEulerAngles(const uint8_t* Data, int Size);

	/// <summary>
	/// BLEマネジャー
	/// </summary>
	UPROPERTY()
	//IBleManagerInterface* BleManager;
	TScriptInterface<IBleManagerInterface> BleManager;

	/// <summary>
	/// 目標のサービス
	/// </summary>
	TArray<FString> Services;

	/// <summary>
	/// 目標のデバイス(今のところ一つしかない)
	/// </summary>
	UPROPERTY()
	//IBleDeviceInterface* MyDevice;
	TScriptInterface<IBleDeviceInterface> MyDevice;

	UPROPERTY()
	EDeviceConnectType State;

	UPROPERTY()
	int devicepitch;

	/// <summary>
	/// ラジアンを度に変換する定数
	/// </summary>
	UFUNCTION()
	void RadianToDegree(float& Radian);

//#if PLATFORM_ANDROID
//	FBleOnDeviceFoundDelegate Function;
//#endif

public:
	/// <summary>
	/// 計算済みのピッチ角を返す関数
	/// </summary>
	/// <returns>デバイスのピッチ角</returns>
	UFUNCTION()
	float DevicePitchAngleGetter();
};
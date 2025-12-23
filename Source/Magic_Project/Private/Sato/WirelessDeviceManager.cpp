// Fill out your copyright notice in the Description page of Project Settings.


#include "Sato/WirelessDeviceManager.h"
#include <array>
#if PLATFORM_ANDROID
#include "BleUtils.h"
#include "Interface/BleDeviceInterface.h"
#include "Interface/BleManagerInterface.h"
#include "AndroidPermissionFunctionLibrary.h"
#include "AndroidPermissionCallbackProxy.h"
#include <Sato/MagicGameInstance.h>
#endif

//DECLARE_DELEGATE(FBleDelegate);
//DECLARE_DELEGATE(FBleErrorDelegate);
//DECLARE_DELEGATE(FBleOnDeviceFoundDelegate);
//DECLARE_DELEGATE(FBleCharacteristicDataDelegate);

UWirelessDeviceManager::UWirelessDeviceManager()
	: BleManager(nullptr)
	, MyDevice(nullptr)
{
	State = EDeviceConnectType::UnConnected;
}

UWirelessDeviceManager::~UWirelessDeviceManager()
{
#if PLATFORM_ANDROID
	if (MyDevice)
	{
		if (State == EDeviceConnectType::Connected)
		{
			Disconnect();
		}
	}

#endif
}

void UWirelessDeviceManager::Init()
{
#if !PLATFORM_ANDROID && !PLATFORM_IOS
	UE_LOG(LogTemp, Error, TEXT("Sorry, this class only support for android or ios platform because of the plugin."));
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("Sorry, this class only support for android or ios platform because of the plugin."));
#endif

	//mahony = NewObject<Mahony>(this);

#if PLATFORM_ANDROID
	BleManager = UBleUtils::CreateBleManager();
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("deviceConnect init"));

	if (!CheckBluetooth())
		return;

	// この以降はデバイスのbluetoothがオンの状態かつBluetooth Low Energy(BLE)がサポートしている状態

	// サービスからデバイスを見つける
	DecideTargetServices();
	// 権限を要求する
	RequestAndroidPermission();
	FindDeviceByServices();
#endif
}

bool UWirelessDeviceManager::Connect()
{
#if PLATFORM_ANDROID
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("Connect()"));
	FBleDelegate SuccFunction;
	SuccFunction.BindUFunction(this, FName("OnConnectSucc"));
	FBleErrorDelegate ErrFunction;
	ErrFunction.BindUFunction(this, FName("OnConnectError"));
	MyDevice.GetInterface()->Connect(SuccFunction, ErrFunction);
	State = EDeviceConnectType::Connecting;
#endif
	return true;
}

bool UWirelessDeviceManager::Disconnect()
{
#if PLATFORM_ANDROID
	FBleDelegate SuccFunction;
	SuccFunction.BindUFunction(this, FName("OnDisconnectSucc"));
	FBleErrorDelegate ErrFunction;
	ErrFunction.BindUFunction(this, FName("OnDisconnectError"));
	MyDevice.GetInterface()->Disconnect(SuccFunction, ErrFunction);
	State = EDeviceConnectType::Disconnecting;
#endif
	return true;
}

//void WirelessDeviceManager::EnableMoveAction_Implementation()
//{
//	MoveSwitch = true;
//}
//
//void WirelessDeviceManager::DisableMoveAction_Implementation()
//{
//	MoveSwitch = false;
//}
//
//void WirelessDeviceManager::EnableSelectAnswerAction_Implementation()
//{
//}
//
//void WirelessDeviceManager::DisableSelectAnswerAction_Implementation()
//{
//}

bool UWirelessDeviceManager::CheckBluetooth()
{
#if PLATFORM_ANDROID
	if (BleManager)
	{
		if (!BleManager.GetInterface()->IsBleSupported())
		{
			UE_LOG(LogTemp, Error, TEXT("This device is not support Bluetooth low energy"));
			return false;
		}

		if (!BleManager.GetInterface()->IsBluetoothEnabled())
		{
			UE_LOG(LogTemp, Warning, TEXT("This device did not open bluetooth"));
			UE_LOG(LogTemp, Warning, TEXT("Open bluetooth"));
			BleManager.GetInterface()->SetBluetoothState(true);
		}
		return true;
	}
#endif

	return false;
}

void UWirelessDeviceManager::RequestAndroidPermission()
{
#if PLATFORM_ANDROID
	if (!UAndroidPermissionFunctionLibrary::CheckPermission(ANDROID_FILE_LOCATION_PERMISSION))
	{
		TArray<FString> Permissions;
		Permissions.Add(ANDROID_FILE_LOCATION_PERMISSION);
		Permissions.Add(ANDROID_BLUETOOTH_CONNECT_PERMISSION);
		Permissions.Add(ANDROID_BLUETOOTH_SCAN_PERMISSION);
		UAndroidPermissionCallbackProxy* Callback = UAndroidPermissionFunctionLibrary::AcquirePermissions(Permissions);
		Callback->OnPermissionsGrantedDynamicDelegate.AddDynamic(this, &UWirelessDeviceManager::OnPermissionResult);
	}
#endif
}

void UWirelessDeviceManager::OnPermissionResult(const TArray<FString>& Permissions, const TArray<bool>& GrantResults)
{
	//#if PLATFORM_ANDROID

	//#endif
}

void UWirelessDeviceManager::DecideTargetServices()
{
	// 少なくとも一つのサービスが必要
	// そうでないと、すべてのデバイスが見つかって戻ってきって、大量のリソースが消耗されるだそうです。
	// https://docs.ninevastudios.com/#/ue-plugins/ble-goodies?id=setup
	UE_LOG(LogTemp, Display, TEXT("Prepare services's uuid"));
	Services.Empty();
	// UUIDで先にデバイスを区別したいが、なぜか何一つデバイスを検知できない
	//Services.Add(IO_IMUSERVICE_UUID);
}

void UWirelessDeviceManager::FindDeviceByServices()
{
#if PLATFORM_ANDROID
	if (BleManager)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("FindDeviceByServices"));
		UE_LOG(LogTemp, Display, TEXT("Do Find Device"));

		FBleOnDeviceFoundDelegate Function;
		Function.BindUFunction(this, FName("OnDeviceFound"));
		BleManager.GetInterface()->ScanForDevices(Services, Function);

		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("ONDeviceFound = -1"));
	}
#endif
}

void UWirelessDeviceManager::OnDeviceFound(TScriptInterface<IBleDeviceInterface> Device)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("ONDeviceFound = 0"));
#if PLATFORM_ANDROID
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("ONDeviceFound = 1"));

	if (Device)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("ONDeviceFound = 2"));
		UE_LOG(LogTemp, Display, TEXT("Found Device: %s"), *Device.GetInterface()->GetDeviceName());

		// デバイスの名前で接続したいデバイスかどうかを判別する
		if (!(Device.GetInterface()->GetDeviceName().Equals(IO_DEVICE_NAME)))
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("ONDeviceFound = 2.5"));
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("FindDeviceName = %s"), *Device.GetInterface()->GetDeviceName()));
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("TargetDeviceName = %s"), *IO_DEVICE_NAME));
			return;
		}
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("ONDeviceFound = 3"));

		// 新しく見つけたデバイスを使用する
		// 既に接続した場合は切断する
		if (MyDevice && State == EDeviceConnectType::Connected)
		{
			Disconnect();
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("ONDeviceFound = 4"));
		}
		else if ((!MyDevice && State == EDeviceConnectType::Connecting) || (MyDevice && State == EDeviceConnectType::Disconnecting))
		{
			if (!MyDevice && State == EDeviceConnectType::Connecting)
				GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Yellow, TEXT("this device is cottecting"));
			else if (MyDevice && State == EDeviceConnectType::Disconnecting)
				GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Yellow, TEXT("this device is Discottecting"));
			// 接続しているか切断しているか
			// 何もしない、やっていることが終了するまで待つ
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("ONDeviceFound = 5"));

		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Connect to Device: %s"), *Device.GetInterface()->GetDeviceName());
			//MyDevice = DeviceInterface;
			MyDevice = Device;

			// 接続する
			Connect();
			//　スキャンを止める
			BleManager.GetInterface()->StopScan();
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("ONDeviceFound = 6"));

		}
	}
#endif
}

void UWirelessDeviceManager::OnConnectSucc()
{
#if PLATFORM_ANDROID
	UE_LOG(LogTemp, Display, TEXT("Connect to device successfully"));
	//Name = MyDevice->GetDeviceName();
	//UUID = MyDevice->GetDeviceId();
	State = EDeviceConnectType::Connected;
	FBleCharacteristicDataDelegate ReceiveFunction;
	ReceiveFunction.BindUFunction(this, FName("OnReceiveData"));
	MyDevice.GetInterface()->BindToCharacteristicNotificationEvent(ReceiveFunction);
	MyDevice.GetInterface()->SubscribeToCharacteristic(IO_IMUSERVICE_UUID, IO_EULER_CHARACTERISTIC_UUID, false);
	FString BatteryUUID = UBleUtils::HexToUUID(IO_BATTERY_CHARACTERISTIC_UUID);
	MyDevice.GetInterface()->SubscribeToCharacteristic(IO_IMUSERVICE_UUID, BatteryUUID, false);
#endif
}

void UWirelessDeviceManager::OnConnectError(FString ErrorMessage)
{
	UE_LOG(LogTemp, Error, TEXT("Connect to device failed: %s"), *ErrorMessage);

	State = EDeviceConnectType::UnConnected;
}

void UWirelessDeviceManager::OnDisconnectSucc()
{
	UE_LOG(LogTemp, Display, TEXT("Disconnect to device successfully"));
	State = EDeviceConnectType::UnConnected;
	MyDevice = nullptr;
}

void UWirelessDeviceManager::OnDisconnectError(FString ErrorMessage)
{
	UE_LOG(LogTemp, Error, TEXT("Disconnect to device failed: %s"), *ErrorMessage);
	State = EDeviceConnectType::Connected;
}

template<typename T>
T UWirelessDeviceManager::TransformDataToInt32(const uint8_t* Data, int Size) const
{
	int32 Result = 0;

	for (int i = 0; i < Size; ++i)
	{
		Result |= (static_cast<int32>(Data[i]) << (8 * (Size - 1 - i)));
	}
	return Result;
}

void UWirelessDeviceManager::OnReceiveData(FString ServiceUUID, FString CharacteristicUUID, TArray<uint8>& Data)
{
	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, FString::Printf(TEXT("Receive %s data"), *CharacteristicUUID));
	if (ServiceUUID.Equals(IO_IMUSERVICE_UUID) && CharacteristicUUID.Equals(IO_EULER_CHARACTERISTIC_UUID))
	{
		HandleRotateData(Data);
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Blue, TEXT("Called HandleRotateData"));
	}
	//else if (ServiceUUID.Equals(IO_BATTERYSERVICE_UUID) && CharacteristicUUID.Equals(IO_BATTERY_CHARACTERISTIC_UUID))
		//HandleBatteryData(Data);
	//else if (ServiceUUID.Equals(IO_PAIRINGSERVICE_UUID) && CharacteristicUUID.Equals())
}

void UWirelessDeviceManager::HandleRotateData(TArray<uint8>& Data)
{
	FRotator WirelessDeviceRotate = TransformEulerAngles(Data.GetData(), Data.Num());
	
	devicepitch = WirelessDeviceRotate.Pitch;
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, FString::Printf(TEXT("Devicepitch = %f"), devicepitch));
}

void UWirelessDeviceManager::HandleBatteryData(const FIMUData& Data)
{
	//uint8 Battery = TransformDataToInt<uint8>(Data.GetData(), Data.Num());
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("Battery: %d%%"), Battery));
}

void UWirelessDeviceManager::DebugReceiveData(const FIMUData& Data)
{
	//FString Content;
	//for (int16_t D : Data)
	//	Content += FString::FromInt(D) + TEXT(", ");
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Content: %s, Num* %d"), *Content, Data.Num()));
}

void UWirelessDeviceManager::RawDataConvertPhysicalValue(const FIMUData& Data)
{
	//FIMUData::accel_x 
}

float UWirelessDeviceManager::DevicePitchAngleGetter()
{
	return devicepitch;
}

// デバイスからもらった情報をTransformDataToInt32に入れて、その結果をFRotatorで返す
FRotator UWirelessDeviceManager::TransformEulerAngles(const uint8_t* Data, int Size)
{
	// バイト配列をfloat配列としてキャスト
	float* FloatData = reinterpret_cast<float*>(const_cast<uint8_t*>(Data));

	float Roll = FloatData[0];
	float Pitch = FloatData[1];
	float Yaw = FloatData[2];

	// floatの生データを表示
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow,
		FString::Printf(TEXT("Float Values - Roll: %f, Pitch:  %f, Yaw: %f"),
			Roll, Pitch, Yaw));

	// ラジアンから度数法に変換
	float RollDeg = Roll * (180.0f / PI);
	float PitchDeg = Pitch * (180.0f / PI);
	float YawDeg = Yaw * (180.0f / PI);

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan,
		FString::Printf(TEXT("Degree Values - Roll: %f, Pitch: %f, Yaw: %f"),
			RollDeg, PitchDeg, YawDeg));

	// FRotatorの引数は（ピッチ、ヨー、ロール）の順
	// まずはラジアンから度に変換した値を使用
	FRotator ResultRotate = FRotator(PitchDeg, YawDeg, RollDeg);
	return ResultRotate;
}

void UWirelessDeviceManager::RadianToDegree(float& Radian)
{
	devicepitch = Radian * (180.0f / PI);
}
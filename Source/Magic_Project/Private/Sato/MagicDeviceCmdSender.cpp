// Fill out your copyright notice in the Description page of Project Settings.


#include "Sato/MagicDeviceCmdSender.h"
#include <array>

FMagicDeviceCmdSender::FMagicDeviceCmdSender()
{
}
FMagicDeviceCmdSender::~FMagicDeviceCmdSender()
{
}

bool FMagicDeviceCmdSender::Init()
{
	// インスタンス化
	deviceInfo_ = NewObject<UASerialPacket>();
	device_ = NewObject<UASerialLibControllerWin>();
	//deviceCmd_ = new FMagicDeviceCmdSender;
	//TSharedPtr<FMagicDeviceCmdSender> deviceCmd_ = MakeShared<FMagicDeviceCmdSender>();
	//deviceCmd_ = MakeShared<FMagicDeviceCmdSender>();

	// デバイス接続
	device_->Initialize(this->DeviceID_, this->DeviceVersion_);
	device_->SetInterfacePt(new WindowsSerial());
	device_->AutoConnectDevice();
	this->SendCmd_Cali(device_);

	return true;
}

uint32 FMagicDeviceCmdSender::Run()
{
	while (StopTaskCounter.GetValue() == 0)
	{
		this->SendCmd_Euler(device_);
		ASerialDataStruct::ASerialData ReceiveData;
		int Result = device_->ReadData(&ReceiveData);

		FRotator Device_Rotate = TransformEulerAngles(ReceiveData.data, 4);
		FRotator Rotate = FRotator(Device_Rotate.Pitch / 1000, Device_Rotate.Yaw / 1000, Device_Rotate.Roll / 1000);

		FScopeLock Lock(&DataMutex);
		DeviceRotate_ = Rotate;
		// ＝＝＝＝＝＝デバッグ情報＝＝＝＝＝＝
		//uint16_t a = device_->GetLastErrorCode();
		//UE_LOG(LogTemp, Log, TEXT("ErrorCode     = %X"), a);
		//UE_LOG(LogTemp, Log, TEXT("deviceCONNECT = %d"), Result);
		//UE_LOG(LogTemp, Log, TEXT("deviceRESULT  = %x"), ReceiveData.data);
		// ＝＝＝＝＝＝デバッグ情報＝＝＝＝＝＝

		FPlatformProcess::Sleep(0.01f);
	}

	return 0;
}

void FMagicDeviceCmdSender::Stop()
{
	StopTaskCounter.Increment(); // これでRunのループを抜けさせる
}

void FMagicDeviceCmdSender::Exit()
{
	device_->DisConnectDevice();
	//delete deviceCmd_;
}

//========================================================================
//================================デバイス================================
//========================================================================

void FMagicDeviceCmdSender::SendCmd_Cali(UASerialLibControllerWin* magicdevice_)
{
	if (magicdevice_)
	{
		magicdevice_->WriteData(Calibration_);
	}
}

int32 FMagicDeviceCmdSender::SendCmd_Euler(UASerialLibControllerWin* magicdevice_)
{
	if (magicdevice_)
	{
		int32 Result = magicdevice_->WriteData(EulerCmd_);
		return Result;
	}
	else
	{
		return -1;
	}
}

int32 FMagicDeviceCmdSender::SendCmd_Quater(UASerialLibControllerWin* magicdevice_)
{
	if (magicdevice_)
	{
		int32 Result = magicdevice_->WriteData(QuaternionCmd_);
		return Result;
	}
	else
	{
		return -1;
	}
}

// デバイスからもらった情報をTransformEulerAnglesからもらって、int32型に変換して返す
int32 FMagicDeviceCmdSender::TransformDataToInt32(const uint8_t* Data, int Size)
{
	int32 Result = 0;

	for (int i = 0; i < Size; ++i)
	{
		Result |= (static_cast<int32>(Data[i]) << (8 * (Size - 1 - i)));
	}
	return Result;
}

// デバイスからもらった情報をTransformDataToInt32に入れて、その結果をFRotatorで返す
FRotator FMagicDeviceCmdSender::TransformEulerAngles(const uint8_t* Data, int Size)
{
	std::array<int32, 3> Angles;
	Angles[0] = TransformDataToInt32(Data, Size);       // X
	Angles[1] = TransformDataToInt32(Data + 4, Size);   // Y
	Angles[2] = TransformDataToInt32(Data + 8, Size);   // Z

	// FRotatorの引数は（ピッチ、ヨー、ロール）の順なのでそれにあわせて番号を変えてる
	FRotator ResultRotate = FRotator(Angles[1], Angles[2], Angles[0]);
	return ResultRotate;
}
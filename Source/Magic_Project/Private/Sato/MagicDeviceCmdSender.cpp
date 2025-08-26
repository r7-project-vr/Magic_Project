// Fill out your copyright notice in the Description page of Project Settings.


#include "Sato/MagicDeviceCmdSender.h"

// Sets default values
AMagicDeviceCmdSender::AMagicDeviceCmdSender()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMagicDeviceCmdSender::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMagicDeviceCmdSender::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMagicDeviceCmdSender::SendCmd_Cali(UASerialLibControllerWin* device_)
{
	if (device_)
	{
		device_->WriteData(Calibration_);
	}
}

int32 AMagicDeviceCmdSender::SendCmd_Euler(UASerialLibControllerWin* device_)
{
	if (device_)
	{
		int32 Result = device_->WriteData(EulerCmd_);
		//int32 Result = device_->WriteData(0x01);
		return Result;
	}
	else
	{
		return -1;
	}
}

int32 AMagicDeviceCmdSender::SendCmd_Quater(UASerialLibControllerWin* device_)
{
	if (device_)
	{
		int32 Result = device_->WriteData(QuaternionCmd_);
		return Result;
	}
	else
	{
		return -1;
	}
}
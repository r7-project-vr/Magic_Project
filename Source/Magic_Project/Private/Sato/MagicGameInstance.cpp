// Fill out your copyright notice in the Description page of Project Settings.


#include "Sato/MagicGameInstance.h"

//void UMagicGameInstance::Init(){}
//void UMagicGameInstance::Shutdown(){}

void UMagicGameInstance::Init()
{
	Super::Init();

#if PLATFORM_WINDOWS
	DeviceManager = NewObject<UDeviceThreadManager>(this);
	DeviceManager->StartManager();
#endif
}

void UMagicGameInstance::Shutdown()
{
	if (DeviceManager)
	{
		DeviceManager->StopManager();
	}

	Super::Shutdown();
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Sato/MagicGameInstance.h"
#include "Sato/DeviceThreadManager.h"

void UMagicGameInstance::Init()
{
	Super::Init();

	DeviceManager = NewObject<UDeviceThreadManager>(this);
	DeviceManager->StartManager();
}

void UMagicGameInstance::Shutdown()
{
	if (DeviceManager)
	{
		DeviceManager->StopManager();
	}

	Super::Shutdown();
}
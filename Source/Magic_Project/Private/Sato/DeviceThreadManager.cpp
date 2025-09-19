// Fill out your copyright notice in the Description page of Project Settings.


#include "Sato/DeviceThreadManager.h"

void UDeviceThreadManager::StartManager()
{
    // ‘¶Ý‚µ‚È‚¢ê‡‚¾‚¯ì‚é
    if (!Runnable)
    {
        Runnable = new FMagicDeviceCmdSender();
        Thread = FRunnableThread::Create(Runnable, TEXT("DeviceRunnableThread"));
    }
}

void UDeviceThreadManager::StopManager()
{
    if (Runnable)
    {
        Runnable->Stop();
    }
    if (Thread)
    {
        Thread->WaitForCompletion();
        delete Thread;
        Thread = nullptr;
    }
    delete Runnable;
    Runnable = nullptr;
}

FRotator UDeviceThreadManager::GetLatestData() const
{

    if (Runnable)
    {
        FScopeLock Lock(&Runnable->DataMutex);
        return Runnable->DeviceRotate_;
    }
    return FRotator(0, 0, 0);
}
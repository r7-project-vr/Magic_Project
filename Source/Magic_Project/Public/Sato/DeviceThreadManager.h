// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "sato/MagicDeviceCmdSender.h"
#include "DeviceThreadManager.generated.h"


UCLASS()
class MAGIC_PROJECT_API UDeviceThreadManager : public UObject
{
	GENERATED_BODY()
	
public:
    // 初期化（スレッド開始）
    void StartManager();

    // 終了処理（スレッド停止）
    void StopManager();

    // デバイスからの最新データを取得
    FRotator GetLatestData() const;

private:
    FMagicDeviceCmdSender* Runnable;
    FRunnableThread* Thread;

    // スレッドで受け取ったデータを保持
    FRotator DeviceInfoResult;

    // ここにキャラクターへ渡す処理を書くこともできる

};

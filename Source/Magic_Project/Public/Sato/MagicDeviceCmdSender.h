// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ASerialLibControllerWin.h"
#include "MagicDeviceCmdSender.generated.h"

UCLASS()
class MAGIC_PROJECT_API AMagicDeviceCmdSender : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMagicDeviceCmdSender();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// キャリブレーションコマンドを送信する関数
	void SendCmd_Cali(UASerialLibControllerWin* device_);

	// オイラー角取得コマンドを送信する関数
	int32 SendCmd_Euler(UASerialLibControllerWin* device_);

	// クォータニオン取得コマンドを送信する関数
	int32 SendCmd_Quater(UASerialLibControllerWin* device_);

private:
	// キャリブレーション取得コマンド
	uint8_t Calibration_ = 0x20;

	// オイラー角取得コマンド
	uint8_t EulerCmd_ = 0x21;

	// クォータニオン取得コマンド
	uint8_t QuaternionCmd_ = 0x22;
};

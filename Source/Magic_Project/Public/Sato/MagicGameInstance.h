// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MagicGameInstance.generated.h"

/**
 * デバイスのマルチスレッドの管理
 * ゲーム難易度の管理
 */
UCLASS()
class MAGIC_PROJECT_API UMagicGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;
	
public:
	UPROPERTY()
	UDeviceThreadManager* DeviceManager;

	UPROPERTY(BluePrintReadWrite)
	int Difficulty;
};

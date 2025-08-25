// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ASerialCore/ASerialPacket.h"
#include "WindowsSerial/WindowsSerial.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ASerialLibControllerWin.generated.h"

enum ConnectResult
{
    Fail = -1,
    Succ
};

/**
 * 
 */
UCLASS(BlueprintType)
class ASERIALCOM_API UASerialLibControllerWin : public UASerialPacket
{
    GENERATED_BODY()

public:

    UASerialLibControllerWin();

    /// UObjectでライフサイクルを管理すると、コンストラクタは必ず引数を持たない状態でなければならない
    /// なので、以下のコンストラクタはイニシャライズに変えます。使う前に必ず呼ぶように

    ///  @brief ASerialオブジェクトを生成する
    ///  @param target_device_id  ターゲットデバイスID
    ///  @param device_ver デバイスのバージョン
    // 2025.08.06 ウー start
    //UASerialLibControllerWin(int target_device_id, int device_ver);
    //UFUNCTION(BlueprintCallable, Category = "ASerial Controller")
    void Initialize(int target_device_id, int device_ver);
    // 2025.08.06 end

    /// @brief ASerialオブジェクトを生成する
    /// @param target_device_id ターゲットデバイスID
    /// @param device_ver_min  デバイスの最小バージョン
    /// @param device_ver_max  デバイスの最大バージョン
    // 2025.08.06 ウー start
    //UASerialLibControllerWin(int target_device_id, int device_ver_min, int device_ver_max);
    //UFUNCTION(BlueprintCallable, Category = "ASerial Controller")
    void Initialize(int target_device_id, int device_ver_min, int device_ver_max);
    // 2025.08.06 end

    /// @brief デバイスに接続する
    /// @param COM_num COM番号
    /// @return  0:接続成功 -1:接続失敗
    //UFUNCTION(BlueprintCallable, Category = "ASerial Controller")
    // 2025.08.06 ウー start
    //int ConnectDevice(int COM_num);
    ConnectResult ConnectDevice(int COM_num);
    // 2025.08.06 end

    /// @brief デバイスに自動接続する
    /// @return 0:接続成功 -1:接続失敗
    /// @note 1~255のCOM番号を順に試行し、接続可能なデバイスを探します。
    //UFUNCTION(BlueprintCallable, Category = "ASerial Controller")
    // 2025.08.06 ウー start
    ConnectResult AutoConnectDevice();
    // 2025.08.06 end

    /// @brief デバイスの接続を切断する 
    /// @return 0:切断成功 -1:切断失敗
    //UFUNCTION(BlueprintCallable, Category = "ASerial Controller")
    // 2025.08.06 ウー start
    ConnectResult DisConnectDevice();
    // 2025.08.06 end

    /// @brief インターフェースポインタを設定
    /// @param interface_pt WindowsSerialのポインタ
    //UFUNCTION(BlueprintCallable, Category = "ASerial Controller")
    void SetInterfacePt(WindowsSerial* interface_pt);

    /// @brief データを読み取り処理
    /// @param read_data_buf  読み取りデータを格納するバッファへのポインタ
    /// @return 0:読み取り中 -1:エラー 1:読み取り完了
    /// @note 読み取り中は0を返し、データが読み取られた場合は1を返します。 
    //UFUNCTION(BlueprintCallable, Category = "ASerial Controller")
    int ReadDataProcess(ASerialDataStruct::ASerialData* read_data_buf);

    /// @brief データを読み取る
    /// @param read_data_buf 読み取りデータを格納するバッファへのポインタ
    /// @return 0:読み取り成功 -1:読み取り失敗 -2:タイムアウㇳ
    int ReadData(ASerialDataStruct::ASerialData* read_data_buf);

    /// @brief データを送信
    /// @param command　コマンド
    /// @param data 書き込むデータ配列
    /// @param data_num　データ数
    /// @return 0:書き込み成功 -1:書き込み失敗
    //UFUNCTION(BlueprintCallable, Category = "ASerial Controller")
    int WriteData(uint8_t command, uint8_t* data, uint8_t data_num);

    /// @brief データを送信(コマンドのみ)
    /// @param command コマンド
    /// @return 0:書き込み成功 -1:書き込み失敗
    //UFUNCTION(BlueprintCallable, Category = "ASerial Controller")
    int WriteData(uint8_t command);

private:
    WindowsSerial* m_inteface = nullptr;
    int m_device_ver_min = 0;
    int m_device_ver_max = 0;

    // 2025.08.06 ウー start
    int m_com = 0;
    // 2025.08.06 end
};

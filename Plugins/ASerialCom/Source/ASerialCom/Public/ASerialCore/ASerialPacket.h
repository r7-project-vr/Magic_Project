// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ASerial_ErrorCodeList.h"
#include <stdint.h>
#include "ASerialPacket.generated.h"

#define _ASERIAL_MAX_DATA_NUM 32

//データ構造体
namespace ASerialDataStruct {

    //ASerialのデータ構造体
    struct ASerialData{
        uint8_t command = 0;
        uint8_t data[_ASERIAL_MAX_DATA_NUM] = { 0 };
        uint8_t data_num = 0;
    };

    //デバイスへのデータ構造体
    struct ToDeviceData {
        uint8_t target_device_id = 0;
        uint8_t data_num = 0;
        uint8_t command = 0;
        uint8_t data[_ASERIAL_MAX_DATA_NUM] = { 0 };
        uint16_t check_data = 0;
    };

    //コントローラへのデータ構造体
    struct ToControllerData {
        uint8_t data_num = 0;
        uint8_t data[_ASERIAL_MAX_DATA_NUM] = { 0 };
        uint16_t check_data = 0;
    };

}

/**
 * 
 */
UCLASS(BlueprintType)
class ASERIALCOM_API UASerialPacket : public UObject
{
    GENERATED_BODY()

public:
    //===public定数===
    static const uint8_t MODE_DEVICE = 0;                   //デバイスモード
    static const uint8_t MODE_CONTROLLER = 1;               //コントローラモード
    static const uint8_t RESERVED_COMMAND_RESET = 0x00;     //予約済みコマンド リセット
    static const uint8_t RESERVED_COMMAND_GET_INFO = 0x01;  //予約済みコマンド デバイス情報リクエスト
    static const uint8_t DATA_NUM_MAX = _ASERIAL_MAX_DATA_NUM;                 //データ最大数
    //================

    UASerialPacket();

    /// UObjectでライフサイクルを管理すると、コンストラクタは必ず引数を持たない状態でなければならない
    /// なので、以下のコンストラクタはイニシャライズに変えます。使う前に必ず呼ぶように
 
    /// @brief ASerialPacketコンストラクタ(deviceモード)
    /// @param _device_id device ID
    /// @param _device_ver device version
    //UASerialPacket(uint8_t _device_id, uint8_t _device_ver);
    void Initialize(uint8_t _device_id, uint8_t _device_ver);

    /// @brief ASerialPacketコンストラクタ(controllerモード)
    /// @param _target_device_id 
    //UASerialPacket(uint8_t _target_device_id);
    void Initialize(uint8_t _target_device_id);


    /// @brief 動作モードを取得
    /// @return 動作モード(MODE_DEVICE or MODE_CONTROLLER)
    int GetMode(void);

    /// @brief IDを取得
    /// @return デバイスIDまたはターゲットデバイスID
    /// @note モードによって取得するIDが異なる。
    uint8_t GetID(void);

    /// @brief Verを取得
    /// @return デバイスVerまたは0(ControllerモードではVerは0)
    /// @note モードによって取得するVerが異なる。
    uint8_t GetVer(void);

    /// @brief 接続状態を取得
    /// @return 接続状態(true or false)
    /// @note コントローラモード時のみ有効。デバイスモードでは常にfalseを返す。
    bool GetConnectionState(void);

    /// @brief 最後に発生したエラーコードを取得
    /// @return 最後に発生したエラーコード
    /// @note エラーコードはASerial::ErrorCodeListに定義されています。
    uint16_t GetLastErrorCode(void);

    /// @brief 必要なパケットバッファサイズを取得
    /// @param data_array データ配列 
    /// @param data_num データ数
    /// @return 必要なパケットバッファサイズ
    size_t GetNeedPacketBufSize(uint8_t* data_array, int data_num);

    /// @brief 接続状態を設定
    /// @param state 接続状態
    void SetConnectionState(bool state);

protected:

    /// @brief パケットデータを読み取り
    /// @param _indata 受信したデータ(1バイト)
    /// @param data_buf_pt 受信したデータを格納するバッファへのポインタ
    /// @return 読み取り中は0、エラーが発生した場合は-1、読み取り完了で1を返す。
    int ReadPacketData(uint8_t _indata, ASerialDataStruct::ASerialData* data_buf_pt);


    /// @brief パケットデータを作成(コントローラモード時)
    /// @param to_device_data デバイスへ送信するデータ配列
    /// @param data_num データ数
    /// @param command 　コマンド
    /// @param data_packet_out パケットデータを格納するバッファへのポインタ
    /// @return 正常時は0、エラー時は-1を返す。
    /// @note コントローラモード時のみ使用可能。デバイスモードでは使用できません。
    /// @note 接続ステータスがfalseの場合、リセットコマンドやデバイス情報リクエスト以外のコマンドはエラーとなります。
    int MakePacketData(uint8_t* to_device_data, int data_num, uint8_t command, uint8_t* data_packet_out);

    /// @brief パケットデータを作成(コントローラモード時)*コマンドのみ
    /// @param command 　コマンド
    /// @param data_packet_out パケットデータを格納するバッファへのポインタ
    /// @return 正常時は0、エラー時は-1を返す。
    /// @note コントローラモード時のみ使用可能。デバイスモードでは使用できません。
    /// @note 接続ステータスがfalseの場合、リセットコマンドやデバイス情報リクエスト以外のコマンドはエラーとなります。
    int MakePacketData(uint8_t command, uint8_t* data_packet_out);


    /// @brief パケットデータを作成(デバイスモード時)
    /// @param to_controller_data コントローラへ送信するデータ配列
    /// @param data_num   データ数
    /// @param data_packet_out パケットデータを格納するバッファへのポインタ
    /// @return 正常時は0、エラー時は-1を返す。
    int MakePacketData(uint8_t* to_controller_data, int data_num, uint8_t* data_packet_out);

private:
    //===private定数===
    static const uint8_t AD_FLAG = 0xAD;      //加算フラグ値
    static const uint8_t DO_FLAG = 0xD0;      //スタートフラグ値
    //================
    uint8_t m_mode = MODE_DEVICE;   //動作モード設定
    uint8_t m_device_id = 0;        //デバイスID
    uint8_t m_device_ver = 0;       //デバイスVer
    uint8_t m_target_device_id = 0; //ターゲットデバイスID
    bool m_add_flag = false;        //加算フラグ
    bool m_read_packet = false;     //packet読み取りフラグ(true中はパケット読み取り中)
    ASerialDataStruct::ToDeviceData m_from_controller_data_buf;          //コントローラからのパケット読み取り中のデータバッファ
    ASerialDataStruct::ToControllerData m_from_device_data_buf;                  //デバイスからのパケット読み取り中のデータバッファ
    bool m_error_flag = false;        //何らかのエラーでパケットを無視するフラグ
    uint16_t m_lase_error_code = static_cast<uint16_t>(ASerial::ErrorCodeList::ERR_NONE);     //最後にエラーになったときのエラーコード
    bool m_connection_state = false;        //コントローラモード時の接続ステータス

    /// @brief フラグをリセット
    void ResetFlags(void);

    /// @brief データ配列をリセット
    void ResetDataArray(void);

    /// @brief データが加算フラグを必要とするかチェックし、必要ならば加算フラグを追加
    int CheckDataNeedAddFlag(uint8_t data, uint8_t* data_array, int* _index);
};

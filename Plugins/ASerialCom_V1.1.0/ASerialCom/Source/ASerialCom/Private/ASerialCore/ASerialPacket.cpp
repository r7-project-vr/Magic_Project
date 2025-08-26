// Fill out your copyright notice in the Description page of Project Settings.


#include "ASerialCore/ASerialPacket.h"

UASerialPacket::UASerialPacket()
{
}

//UASerialPacket::UASerialPacket(uint8_t _device_id, uint8_t _device_ver)
//{
//    m_device_id = _device_id;
//    m_device_ver = _device_ver;
//    m_mode = MODE_DEVICE;
//}
//
//UASerialPacket::UASerialPacket(uint8_t _target_device_id)
//{
//    m_target_device_id = _target_device_id;
//    m_mode = MODE_CONTROLLER;
//}

void UASerialPacket::Initialize(uint8_t _device_id, uint8_t _device_ver)
{
    m_device_id = _device_id;
    m_device_ver = _device_ver;
    m_mode = MODE_DEVICE;
}

void UASerialPacket::Initialize(uint8_t _target_device_id)
{
    m_target_device_id = _target_device_id;
    m_mode = MODE_CONTROLLER;
}

int UASerialPacket::GetMode(void) { return m_mode; }

uint8_t UASerialPacket::GetID(void)
{
    uint8_t ret = 0;
    if (GetMode() == MODE_DEVICE) {
        ret = m_device_id;
    }
    else {
        ret = m_target_device_id;
    }

    return ret;
}

uint8_t UASerialPacket::GetVer(void)
{
    uint8_t ret = 0;
    if (GetMode() == MODE_DEVICE) {
        ret = m_device_ver;
    }
    else {
        ret = 0;
    }

    return ret;
}

bool UASerialPacket::GetConnectionState(void)
{
    bool state = false;
    if (GetMode() == MODE_CONTROLLER) {
        state = m_connection_state;
    }
    else {
        state = false;
    }
    return state;
}

uint16 UASerialPacket::GetLastErrorCode(void) { return m_lase_error_code; }

size_t UASerialPacket::GetNeedPacketBufSize(uint8_t* data_array, int data_num)
{
    size_t size = 0;

    if (GetMode() == MODE_DEVICE) {
        size += 2;  // スタートフラグ(1) + データ数(1)
    }
    else {
        size += 4;  // スタートフラグ(1) + ターゲットデバイスID(1) + データ数(1) + コマンド(1)
    }

    for (int i = 0; i < data_num; ++i) {
        if (data_array[i] == AD_FLAG || data_array[i] == DO_FLAG) {
            size += 2;  // 加算フラグ(1) + データ(1)
        }
        else {
            size += 1;  // データ(1)
        }
    }

    size += 2;  // チェックデータ

    return size;
}

void UASerialPacket::SetConnectionState(bool state) { m_connection_state = state; }

//===protected===

int UASerialPacket::ReadPacketData(uint8_t _indata, ASerialDataStruct::ASerialData* data_buf_pt)
{
    int ret_st = 0;                          // リターンステート
    static uint8_t step = 0;                 // パケット読み取り位置管理
    static uint8_t data_read_count = 0;      // データ読み取り数カウント
    static uint16_t check_data_sum = 0;      // チェック用データ加算変数
    static uint8_t check_data_buf[2] = { 0 };  // 2データ分のデータを読むためのバッファ

    if (_indata == DO_FLAG) {         // スタートフラグ
        if (m_read_packet == true) {  // パケットの読み飛ばし発生警告
            m_lase_error_code = static_cast<uint16_t>(ASerial::ErrorCodeList::WARNING_READ_SKIP);
        }

        //===各フラグやバッファの初期化===
        ResetFlags();
        ResetDataArray();
        if (m_mode == MODE_DEVICE) {
            m_from_controller_data_buf.command = 0;
        }
        m_read_packet = true;
        step = 0;
        data_read_count = 0;
        check_data_sum = 0;

        ret_st = 0;
    }
    else if (m_error_flag == true) {  // エラーがあったパケットを読み飛ばす
        m_read_packet = false;
        ret_st = 0;
    }
    else if (_indata == AD_FLAG) {  // 加算フラグ
        if (m_add_flag == true) {
            ret_st = -1;
            m_error_flag = true;
            m_read_packet = false;
            m_lase_error_code = static_cast<uint16_t>(ASerial::ErrorCodeList::ERR_ADDFLAG_CASCADE);
        }
        else {
            ret_st = 0;
            m_add_flag = true;
        }
    }
    else if (m_read_packet == true) {
        if (m_add_flag == true) {  // 加算処理
            _indata += 1;
            m_add_flag = false;
        }

        if (GetMode() == MODE_DEVICE) {  // デバイスモード
            switch (step) {
            case 0:  // ターゲットデバイスID読み取り
                m_from_controller_data_buf.target_device_id = _indata;

                step = 1;
                break;

            case 1:  // 送信データ数
                m_from_controller_data_buf.data_num = _indata;
                if (m_from_controller_data_buf.data_num > DATA_NUM_MAX) {
                    m_lase_error_code = static_cast<uint16_t>(ASerial::ErrorCodeList::ERR_DATA_COUNT_INFO_OVER);
                    m_error_flag = true;
                    ret_st = -1;
                }
                else {
                    ret_st = 0;
                }
                step = 2;
                break;

            case 2:  // コマンド
                m_from_controller_data_buf.command = _indata;
                // ここでデバイス情報リクエストなのかの判定を行い、デバイスIDとターゲットデバイスIDが不一致かつデバイス情報リクエスまたはリセットコマンドではなかった場合パケットを破棄する
                if ((m_from_controller_data_buf.command != RESERVED_COMMAND_RESET) &&
                    (m_from_controller_data_buf.command != RESERVED_COMMAND_GET_INFO) &&
                    (m_from_controller_data_buf.target_device_id != m_device_id)) {
                    m_error_flag = true;
                    m_lase_error_code = static_cast<uint16_t>(ASerial::ErrorCodeList::ERR_DEVICE_ID_MISMATCH);
                    ret_st = -1;
                }
                else {
                    ret_st = 0;
                }

                if (m_from_controller_data_buf.data_num <= 0) {  // データが0個ならチェックデータ読み取りへ
                    step = 4;
                }
                else {
                    step = 3;
                }

                break;

            case 3:  // データ読み取り
                m_from_controller_data_buf.data[data_read_count] = _indata;
                check_data_sum += _indata;

                if (data_read_count >= (m_from_controller_data_buf.data_num - 1)) {
                    step = 4;
                }

                ++data_read_count;
                ret_st = 0;
                break;

            case 4:  // チェックデータ1
                check_data_buf[0] = _indata;

                step = 5;

                ret_st = 0;
                break;

            case 5:  // チェックデータ2
                check_data_buf[1] = _indata;
                uint16_t check_data = (((uint16_t)check_data_buf[0] << 8) | check_data_buf[1]);
                if (check_data_sum != check_data) {
                    ret_st = -1;
                    m_error_flag = true;
                    m_lase_error_code = static_cast<uint16_t>(ASerial::ErrorCodeList::ERR_CHECK_DATA_MISMATCH);
                }
                else {
                    ret_st = 1;
                    m_read_packet = false;

                    // 読み取ったデータを戻り用データ構造体に格納
                    data_buf_pt->command = m_from_controller_data_buf.command;
                    data_buf_pt->data_num = m_from_controller_data_buf.data_num;
                    for (int i = 0; i < DATA_NUM_MAX; ++i) {
                        data_buf_pt->data[i] = m_from_controller_data_buf.data[i];
                    }
                }
                break;
            }
        }
        else {  // コントローラモード
            switch (step) {
            case 0:  // 送信データ数の読み取り
                m_from_device_data_buf.data_num = _indata;
                if (m_from_device_data_buf.data_num > DATA_NUM_MAX) {
                    m_lase_error_code = static_cast<uint16_t>(ASerial::ErrorCodeList::ERR_DATA_COUNT_INFO_OVER);
                    m_error_flag = true;
                    ret_st = -1;
                }
                else {
                    ret_st = 0;
                }
                step = 1;
                break;
            case 1:  // データ読み取り
                m_from_device_data_buf.data[data_read_count] = _indata;
                check_data_sum += _indata;

                if (data_read_count >= (m_from_device_data_buf.data_num - 1)) {
                    step = 2;
                }

                ++data_read_count;
                ret_st = 0;
                break;

            case 2:  // チェックデータ1
                check_data_buf[0] = _indata;

                step = 3;

                ret_st = 0;
                break;

            case 3:  // チェックデータ2
                check_data_buf[1] = _indata;
                uint16_t check_data = (((uint16_t)check_data_buf[0] << 8) | check_data_buf[1]);
                if (check_data_sum != check_data) {
                    ret_st = -1;
                    m_error_flag = true;
                    m_lase_error_code = static_cast<uint16_t>(ASerial::ErrorCodeList::ERR_CHECK_DATA_MISMATCH);
                }
                else {
                    ret_st = 1;
                    m_read_packet = false;

                    // 読み取ったデータを戻り用データ構造体に格納
                    data_buf_pt->command = 0x00;
                    data_buf_pt->data_num = m_from_device_data_buf.data_num;
                    for (int i = 0; i < DATA_NUM_MAX; ++i) {
                        data_buf_pt->data[i] = m_from_device_data_buf.data[i];
                    }
                }
                break;
            }
        }
    }
    else {
        ret_st = -1;
    }

    return ret_st;
}

int UASerialPacket::MakePacketData(uint8_t* to_device_data, int data_num, uint8_t command, uint8_t* data_packet_out)
{
    if (GetMode() != MODE_CONTROLLER) {  // デバイスモード時にコントローラのパケットを作成を制限
        return -1;
    }

    if (GetConnectionState() == false && command != RESERVED_COMMAND_GET_INFO &&
        command !=
        RESERVED_COMMAND_RESET) {  // 接続状態がfalseかつコマンドがリセットやデバイス情報リクエストでない場合はエラー
        return -1;
    }

    if (data_packet_out == nullptr) {  // バッファがnullptrだったらエラー
        return -1;
    }

    int index = 0;

    data_packet_out[index] = DO_FLAG;  // スタートフラグ
    ++index;
    CheckDataNeedAddFlag(m_target_device_id, data_packet_out, &index);  // ターゲットデバイスID
    ++index;
    CheckDataNeedAddFlag((uint8_t)data_num, data_packet_out, &index);  // データ数
    ++index;
    CheckDataNeedAddFlag(command, data_packet_out, &index);  // コマンド
    ++index;

    uint16_t check_sum = 0;

    for (int i = 0; i < data_num; ++i) {
        check_sum += to_device_data[i];
        CheckDataNeedAddFlag(to_device_data[i], data_packet_out, &index);
        ++index;
    }

    int8_t check_data[2] = { 0 };  // チェックデータ分割用([1]上位バイト [2]下位バイト)

    check_data[1] = (uint8_t)((check_sum & 0xFF00) >> 8);  // 上位バイト抽出
    check_data[2] = (uint8_t)(check_sum & 0x00FF);         // 下位バイト抽出

    CheckDataNeedAddFlag(check_data[1], data_packet_out, &index);
    ++index;
    CheckDataNeedAddFlag(check_data[2], data_packet_out, &index);

    return 0;
}

int UASerialPacket::MakePacketData(uint8_t command, uint8_t* data_packet_out)
{
    if (GetMode() != MODE_CONTROLLER) {  // デバイスモード時にコントローラのパケットを作成を制限
        return -1;
    }

    if (GetConnectionState() == false && command != RESERVED_COMMAND_GET_INFO &&
        command !=
        RESERVED_COMMAND_RESET) {  // 接続状態がfalseかつコマンドがリセットやデバイス情報リクエストでない場合はエラー
        return -1;
    }

    if (data_packet_out == nullptr) {  // バッファがnullptrだったらエラー
        return -1;
    }

    int index = 0;

    data_packet_out[index] = DO_FLAG;  // スタートフラグ
    ++index;
    CheckDataNeedAddFlag(m_target_device_id, data_packet_out, &index);  // ターゲットデバイスID
    ++index;
    data_packet_out[index] = 0x00;  // データ数
    ++index;
    CheckDataNeedAddFlag(command, data_packet_out, &index);  // コマンド
    ++index;
    data_packet_out[index] = 0x00;
    ++index;
    data_packet_out[index] = 0x00;

    return 0;
}

int UASerialPacket::MakePacketData(uint8_t* to_controller_data, int data_num, uint8_t* data_packet_out)
{
    if (GetMode() != MODE_DEVICE) {  // コントローラモード時にデバイスのパケットを作成を制限
        return -1;
    }

    if (data_packet_out == nullptr) {  // バッファがnullptrだったらエラー
        return -1;
    }

    int index = 0;

    data_packet_out[index] = DO_FLAG;  // スタートフラグ
    ++index;
    CheckDataNeedAddFlag(data_num, data_packet_out, &index);  // データ数
    ++index;

    uint16_t check_sum = 0;

    for (int i = 0; i < data_num; ++i) {
        check_sum += to_controller_data[i];
        CheckDataNeedAddFlag(to_controller_data[i], data_packet_out, &index);
        ++index;
        // Serial.println(st);
    }

    int8_t check_data[2] = { 0 };  // チェックデータ分割用([1]上位バイト [2]下位バイト)

    check_data[1] = (uint8_t)((check_sum & 0xFF00) >> 8);  // 上位バイト抽出
    check_data[2] = (uint8_t)(check_sum & 0x00FF);         // 下位バイト抽出

    CheckDataNeedAddFlag(check_data[1], data_packet_out, &index);
    ++index;
    CheckDataNeedAddFlag(check_data[2], data_packet_out, &index);

    return 0;
}

//===private===

void UASerialPacket::ResetFlags(void)
{
    m_add_flag = false;
    m_read_packet = false;
    m_error_flag = false;
}

void UASerialPacket::ResetDataArray(void)
{
    uint8_t* array = nullptr;

    if (GetMode() == MODE_DEVICE) {
        array = m_from_device_data_buf.data;
    }
    else {
        array = m_from_controller_data_buf.data;
    }

    int data_num = sizeof(array) / sizeof(array[0]);
    for (int i = 0; i < data_num; ++i) {
        array[i] = (uint8_t)0;
    }

    return;
}

int UASerialPacket::CheckDataNeedAddFlag(uint8_t data, uint8_t* data_array, int* _index)
{
    if (data_array == nullptr) {
        return -1;
    }

    int ret = 0;

    if (data == AD_FLAG || data == DO_FLAG) {
        data_array[*_index] = AD_FLAG;
        *_index += 1;
        data_array[*_index] = (data - 1);
        ret = 1;
    }
    else {
        data_array[*_index] = data;
        ret = 0;
    }

    return ret;
}

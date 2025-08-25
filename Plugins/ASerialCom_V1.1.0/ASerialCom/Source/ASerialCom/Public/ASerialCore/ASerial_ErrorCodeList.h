// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <stdint.h>

#ifndef ASERIAL_ERRPRCODELIST_H
#define ASERIAL_ERRPRCODELIST_H

//ErrorCode 0x0000 ~ 0xFFFF
namespace ASerial {
    enum class ErrorCodeList : uint16_t {
        ERR_NONE = 0x0000,             //正常
        WARNING_READ_SKIP = 0x0010,         //パケットの読み飛ばし発生(警告)
        ERR_ADDFLAG_CASCADE = 0x0020,       //加算フラグ連続エラー
        ERR_DEVICE_ID_MISMATCH = 0x0021,    //デバイスIDとターゲットデバイスIDの不一致
        ERR_DATA_COUNT_INFO_OVER = 0x0022,  //送信データ数情報がデータ数上限を超過
        ERR_CHECK_DATA_MISMATCH = 0x0023    //チェックデータの不整合
    };
}



#endif //ASERIAL_ERRPRCODELIST_H
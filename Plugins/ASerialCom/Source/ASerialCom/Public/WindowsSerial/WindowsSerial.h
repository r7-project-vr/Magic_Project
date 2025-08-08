// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>

#ifndef WINDOWS_SERIAL_H
#define WINDOWS_SERIAL_H

/**
 * 
 */
class ASERIALCOM_API WindowsSerial
{
public:
    /// @brief WindowsSerialコンストラクタ
    /// @param baudrate 通信速度(ボーレート) *省略可
    /// @note デフォルトは115200bps
    WindowsSerial(int baudrate = ASERIAL_BAUDRATE_NORMAL);

    /// @brief シリアルポートをオープンする
    /// @param com_num COM番号
    /// @param receive_buffer 受信バッファサイズ(byte)*省略可
    /// @param transmit_buffer 送信バッファサイズ(byte)*省略可
    /// @param read_interval_timeout 読み取りインターバルタイム(msec)*省略可
    /// @param read_timeout 読み取りタイムアウト(msec)*省略可
    /// @param write_timeout 書き込みタイムアウト(msec)*省略可
    /// @return 0:オープン成功
    /// @return -1:オープン失敗
    /// @return -2:通信設定失敗
    /// @return -3:受信バッファ設定失敗
    /// @return -4:タイムアウト設定失敗
    int OpenPort(int com_num,
        int receive_buffer = ASERIAL_RECEIVEBUFFER_NORMAL,
        int transmit_buffer = ASERIAL_TRANSMITBUFFER_NOMAL,
        int read_interval_timeout = ASERIAL_READ_INTERVAL_TIMEOUT_NORMAL,
        int read_timeout = ASERIAL_READTIMEOUT_NORMAL,
        int write_timeout = ASERIAL_WRITETIMEOUT_NORMAL);

    //  @brief  シリアルポートをクローズする
    //  @return 0:成功 -1:失敗
    int ClosePort(void);

    /// @brief ポートの接続状態を取得
    /// @return true:接続中 false:未接続
    bool GetState(void);

    /// @brief 接続しているCOM番号を取得 
    /// @return 0:未接続 0以外:ポート番号
    int GetConnectCOM(void);

    /// @brief 受信バッファに溜まっているデータ数を取得
    /// @return -1:未接続 0:データなし 1以上:データ数
    int available(void);

    /// @brief データを読み取る
    /// @return -1:未接続 -1以外:読み取ったデータ
    int read(void);

    /// @brief データを書き込む
    /// @param val 書き込むデータ
    /// @return -1:未接続 -1以外:書き込んだバイト数
    int write(uint8_t val);

    /// @brief 文字列を書き込む
    /// @param str 書き込む文字列
    /// @return -1:未接続 -1以外:書き込んだバイト数
    int write(std::string str);

    /// @brief データを書き込む
    /// @param buf 書き込むデータのポインタ
    /// @param len 書き込むデータの長さ
    /// @return -1:未接続 -1以外:書き込んだバイト数
    int write(uint8_t* buf, int len);

    /// @brief バッファをクリアする
    /// @return 0:成功 -1:失敗
    int clear(void);

private:
    //===定数定義(private)===
    static const int ASERIAL_BAUDRATE_NORMAL = 115200;           // デフォルトのボーレート
    static const int ASERIAL_RECEIVEBUFFER_NORMAL = 1024;        // デフォルト受信バッファメモリサイズ[byte]
    static const int ASERIAL_TRANSMITBUFFER_NOMAL = 1024;        // デフォルト送信バッファメモリサイズ[byte]
    static const int ASERIAL_READTIMEOUT_NORMAL = 200;           // デフォルト受信タイムアウト時間[ms]
    static const int ASERIAL_WRITETIMEOUT_NORMAL = 200;          // デフォルト送信タイムアウト時間[ms]
    static const int ASERIAL_READ_INTERVAL_TIMEOUT_NORMAL = 50;  // デフォルト読み込みインターバルタイム

    HANDLE m_serial_handle = nullptr;  // シリアル通信ハンドル
    int m_connect_comnum = 0;          // 接続しているポート番号
    bool m_connect_state = false;  // 接続ステータス
    int m_baudrate = 0;                // 通信速度(ボーレート)

    // @brief   通信の設定を行う(内部完結)
    // @param   baudrate 通信速度(ボーレート)
    int ComSetting(int baudrate);

    // @brief   送受信バッファの設定
    // @param   receive_buffer 受信のバッファーサイズ(byte)
    // @param   transmit_buffer   送信のバッファーサイズ(byte)
    int SetBuffer(int receive_buffer, int transmit_buffer);

    // @brief   タイムアウト含む時間関係の設定
    // @param   read_interval_timeout   文字読込時の二文字間の全体待ち時間(msec)
    // @param   read_timeout   読込エラー検出用のタイムアウト時間(msec)
    // @param   write_timeout  書き込みエラー検出用のタイムアウト時間(msec)
    int SetTimeout(int read_interval_timeout, int read_timeout, int write_timeout);
};

#endif
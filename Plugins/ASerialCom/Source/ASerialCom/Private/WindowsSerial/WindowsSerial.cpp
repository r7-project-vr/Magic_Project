// Fill out your copyright notice in the Description page of Project Settings.


#include "WindowsSerial/WindowsSerial.h"

// public
WindowsSerial::WindowsSerial(int baudrate) { m_baudrate = baudrate; }

int WindowsSerial::OpenPort(int com_num,
    int receive_buffer,
    int transmit_buffer,
    int read_interval_timeout,
    int read_timeout,
    int write_timeout)
{
    if (GetState() == true) {
        ClosePort();
    }

    int st = 0;

    char com_name[20];

    if (com_num >= 10) {  // COM番号が10以上の場合、\\.\COMx形式で指定
        sprintf_s(com_name, 20, "\\\\.\\COM%d\0", com_num);
    }
    else {
        sprintf_s(com_name, 20, "COM%d\0", com_num);
    }

    m_serial_handle = CreateFileA(
        com_name,                       // 　　ポートの名前： どのポートを開くのか
        GENERIC_READ | GENERIC_WRITE,   // 　アクセスモード：　通常送受信ともするので読書き両方を指定
        0,                              // 　　共有モード：　通常0に設定　再オープン禁止
        NULL,                           // セキュリティアトリビュート：通信では通常NULLに設定　
        OPEN_EXISTING,                  // 　クリエイトﾃﾞｨｽﾄﾘビューション：通常COMポートはすでに存在しているのでOPEN_EXISTINGとします。
        FILE_ATTRIBUTE_NORMAL,          // 　属性：通信の場合属性はないのでFILE_ATTRIBUTE_NORMAL（属性なし）を指定　
        NULL                            // 　テンプレートのハンドル：　通信の場合関係ない　通常NULLを指定
    );

    if (m_serial_handle == INVALID_HANDLE_VALUE) {  // ハンドル取得に失敗した場合
        return -1;
    }

    st = ComSetting(m_baudrate);
    if (st == -1) {
        ClosePort();
        return -2;
    }

    st = SetBuffer(receive_buffer, transmit_buffer);
    if (st == -1) {
        ClosePort();
        return -3;
    }

    st = SetTimeout(read_interval_timeout, read_timeout, write_timeout);
    if (st == -1) {
        ClosePort();
        return -4;
    }

    m_connect_comnum = com_num;  // 接続しているポート番号を更新
    m_connect_state = true;      // 接続ステータスを更新
    return 0;
}

int WindowsSerial::ClosePort(void)
{
    int ret = CloseHandle(m_serial_handle);
    m_connect_comnum = 0;     // 接続しているポート番号をリセット
    m_connect_state = false;  // 接続ステータスを更新
    if (ret == FALSE) {
        return -1;
    }

    return 0;
}

bool WindowsSerial::GetState(void) { return m_connect_state; }

int WindowsSerial::GetConnectCOM(void) { return m_connect_comnum; }

int WindowsSerial::available(void)
{
    if (GetState() == false) {
        return -1;
    }

    COMSTAT ComStat;

    ClearCommError(m_serial_handle, NULL, &ComStat);

    return (int)ComStat.cbInQue;
}

int WindowsSerial::read(void)
{
    if (GetState() == false) {
        return -1;
    }

    int8_t read_data;
    DWORD dwSendSize;

    int ret = ReadFile(   // データの受信
        m_serial_handle,  // 　通信デバイスのハンドル：　CreateFile()で取得したハンドルを指定
        &read_data,       // 受信バッファーのポインタを指定：　受信データがここに格納されます。
        1,  // 　受信するバイト数を指定：　ここで指定するバイト数を受信するかまたはタイムアウト時間がくるまで
        // ReadFile()関数は（　getc()のように　）待ちます
        &dwSendSize,  //  実際に受信したバイト数（DWORD)が格納されるポインタを指定
        NULL          // 通信とは関係ない引数なのでNULLを指定
    );

    if (ret == FALSE || dwSendSize != 1) {
        return -1;
    }

    return static_cast<int>(read_data);
}

int WindowsSerial::write(uint8_t val)
{
    if (GetState() == false) {
        return -1;
    }

    DWORD dwSendSize;

    int Ret = WriteFile(  // データの送信
        m_serial_handle,  // 　通信デバイスのハンドル：CreateFile()で取得したハンドルを指定
        &val,             // 　送信データのポインタを指定
        1,                // 　送信するデータのバイト数を指定
        &dwSendSize,      //  実際に送信されたバイト数（DWORD)が格納されるポインタを指定
        NULL              // 　　通信とは関係ない引数なのでNULLを指定　　　　
    );

    if (dwSendSize != 1 || Ret == FALSE) {
        return -1;
    }

    return static_cast<int>(dwSendSize);
}

int WindowsSerial::write(std::string str)
{
    if (GetState() == false) {
        return -1;
    }

    DWORD dwSendSize;

    int Ret = WriteFile(  // データの送信
        m_serial_handle,  // 　通信デバイスのハンドル：CreateFile()で取得したハンドルを指定
        str.c_str(),      // 　送信データのポインタを指定
        str.length(),     // 　送信するデータのバイト数を指定
        &dwSendSize,      //  実際に送信されたバイト数（DWORD)が格納されるポインタを指定
        NULL              // 　　通信とは関係ない引数なのでNULLを指定　　　　
    );

    if (dwSendSize != str.length() || Ret == FALSE) {
        return -1;
    }

    return static_cast<int>(dwSendSize);
}

int WindowsSerial::write(uint8_t* buf, int len)
{
    if (GetState() == false) {
        return -1;
    }

    DWORD dwSendSize;

    int Ret = WriteFile(  // データの送信
        m_serial_handle,  // 　通信デバイスのハンドル：CreateFile()で取得したハンドルを指定
        buf,              // 　送信データのポインタを指定
        len,              // 　送信するデータのバイト数を指定
        &dwSendSize,      //  実際に送信されたバイト数（DWORD)が格納されるポインタを指定
        NULL              // 　　通信とは関係ない引数なのでNULLを指定　　　　
    );

    if (dwSendSize != len || Ret == FALSE) {
        return -1;
    }

    return static_cast<int>(dwSendSize);
}

int WindowsSerial::clear(void)
{
    if (GetState() == false) {
        return -1;
    }

    int ret = PurgeComm(  // 消去
        m_serial_handle,  // 　通信デバイスのハンドル：CreateFile()で取得したハンドルを指定
        PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR
        //   実行する操作： 上記は未処理の読書きの中止及び送受信のバッファーのクリアを指定
    );

    if (ret == FALSE) {
        return -1;
    }

    return 0;
}

//===private===

int WindowsSerial::ComSetting(int baudrate)
{
    DCB dcb;

    // 通信の設定
    GetCommState(m_serial_handle, &dcb);

    dcb.DCBlength = sizeof(DCB);  // 　 DCBのサイズ：

    // データ
    dcb.BaudRate = baudrate;  //  伝送速度：　ボーレートをbps単位で指定
    dcb.fBinary = TRUE;       //    バイナリモード：  通常TRUEに設定
    dcb.ByteSize = 8;         // 　   データサイズ　：　通常　8 ビット
    dcb.fParity = NOPARITY;   //  パリティ有無種類　：　パリティなしの場合はNOPARITYを指定
    //                                  奇数パリティの場合は ODDPARITY 　他
    dcb.StopBits = ONESTOPBIT;  // ストップビットの数：　通常１ビット→ ONESTOPBIT;

    // ハードウェアフロー制御
    dcb.fOutxCtsFlow = FALSE;  // 　CTSハードウェアフロー制御：CTS制御を使用しない場合はFLASEを指定
    //   　　　　　　CTS 制御をする場合はTRUEを指定してCTS信号を監視します。
    dcb.fOutxDsrFlow = FALSE;               //  DSRハードウェアフロー制御：使用しない場合はFALSEを指定
    dcb.fDtrControl = DTR_CONTROL_DISABLE; // DTR有効/無効：　無効なら　DTR_CONTROL_DISABLE;ISABLE
    dcb.fRtsControl = RTS_CONTROL_DISABLE;  // RTS制御：　RTS制御をしない場合はRTS_CONTROL_DISABLEを指定
    // 　　　　　　　 RTS制御をする場合はRTS_CONTROL_ENABLE　等を指定

    // ソフトウェアフロー制御
    dcb.fOutX = FALSE;             // 送信時XON/OFF制御の有無：　なし→FLALSE
    dcb.fInX = FALSE;              // 受信時XON/XOFF制御の有無：なし→FALSE
    dcb.fTXContinueOnXoff = TRUE;  // 受信バッファー満杯＆XOFF受信後の継続送信可否：送信可→TRUE
    dcb.XonLim = 512;              // XONが送られるまでに格納できる最小バイト数：
    dcb.XoffLim = 512;             // XOFFが送られるまでに格納できる最小バイト数：
    dcb.XonChar = 0x11;            // 送信時XON文字 ( 送信可：ビジィ解除 ) の指定：
    // 　一般に、XON文字として11H ( デバイス制御１：DC1 )がよく使われます
    dcb.XoffChar = 0x13;  // XOFF文字（送信不可：ビジー通告）の指定：なし→FALSE
    // 　一般に、XOFF文字として13H ( デバイス制御3：DC3 )がよく使われます

    // その他
    dcb.fNull = TRUE;          // NULLバイトの破棄： 破棄する→TRUE
    dcb.fAbortOnError = TRUE;  // エラー時の読み書き操作終了：　終了する→TRUE
    dcb.fErrorChar = FALSE;    // パリティエラー発生時のキャラクタ（ErrorChar）置換：　なし→FLALSE
    dcb.ErrorChar = 0x00;      // パリティエラー発生時の置換キャラクタ
    dcb.EofChar = 0x03;        // データ終了通知キャラクタ　：　一般に0x03(ETX)がよく使われます。
    dcb.EvtChar = 0x02;        // イベント通知キャラクタ ：　一般に0x02(STX)がよく使われます

    int Ret =
        SetCommState(m_serial_handle, &dcb);  // SetCommState()関数にポートハンドルおよびdcb構造体のアドレスを代入する

    if (Ret == FALSE) {
        return -1;
    }



    return 0;
}

int WindowsSerial::SetBuffer(int receive_buffer, int transmit_buffer)
{
    int Ret = SetupComm(  // 設定
        m_serial_handle,  // 　通信デバイスのハンドル：CreateFile()で取得したハンドルを指定
        receive_buffer,   //   受信バッファーサイズ：　受信のバッファーサイズをバイト単位で指定
        transmit_buffer   // 　送信バッファーサイズ：　送信のバッファーサイズをバイト単位で指定
    );

    if (Ret == FALSE) {  // 　失敗した場合
        return -1;
    }

    return 0;
}

int WindowsSerial::SetTimeout(int read_interval_timeout, int read_timeout, int write_timeout)
{
    COMMTIMEOUTS timeout;

    timeout.ReadIntervalTimeout = read_interval_timeout;  // 文字読込時の二文字間の全体待ち時間（msec）

    timeout.ReadTotalTimeoutMultiplier = 0;           // 読込の１文字あたりの時間
    timeout.ReadTotalTimeoutConstant = read_timeout;  // 読込エラー検出用のタイムアウト時間
    //(受信トータルタイムアウト) = ReadTotalTimeoutMultiplier * (受信予定バイト数) + ReadTotalTimeoutConstant

    timeout.WriteTotalTimeoutMultiplier = 0;            // 書き込み１文字あたりの待ち時間
    timeout.WriteTotalTimeoutConstant = write_timeout;  // 書き込みエラー検出用のタイムアウト時間
    //(送信トータルタイムアウト) = WriteTotalTimeoutMultiplier * (送信予定バイト数) + WriteTotalTimeoutConstant

    int Ret =
        SetCommTimeouts(m_serial_handle, &timeout);  // SetCommTimeOut()関数にポートハンドルおよびCOMMTIMEOUTS構造体の
    // アドレスを代入します。

    if (Ret == FALSE) {  // 失敗した場合
        return -1;
    }

    return 0;
}
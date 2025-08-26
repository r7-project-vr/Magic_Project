// Fill out your copyright notice in the Description page of Project Settings.


#include "ASerialLibControllerWin.h"

UASerialLibControllerWin::UASerialLibControllerWin()
{
}

// 2025.08.06 ウー start
//UASerialLibControllerWin::UASerialLibControllerWin(int target_device_id, int device_ver)
//    : UASerialPacket(target_device_id)
//{
//    m_device_ver_max = device_ver;
//    m_device_ver_min = device_ver;
//}
//
//UASerialLibControllerWin::UASerialLibControllerWin(int target_device_id, int device_ver_min, int device_ver_max)
//    : UASerialPacket(target_device_id)
//{
//    m_device_ver_max = device_ver_max;
//    m_device_ver_min = device_ver_min;
//}
// 2025.08.06 end

void UASerialLibControllerWin::Initialize(int target_device_id, int device_ver)
{
    Super::Initialize(target_device_id);
    m_device_ver_max = device_ver;
    m_device_ver_min = device_ver;
}

void UASerialLibControllerWin::Initialize(int target_device_id, int device_ver_min, int device_ver_max)
{
    Super::Initialize(target_device_id);
    m_device_ver_max = device_ver_max;
    m_device_ver_min = device_ver_min;
}

ConnectResult UASerialLibControllerWin::ConnectDevice(int COM_num)
{
    if (GetConnectionState() == true) {
        return ConnectResult::Fail;
    }

    int st = m_inteface->OpenPort(COM_num);
    if (st != 0) {
        return ConnectResult::Fail;
    }

    m_inteface->clear(); //接続したときにバッファをクリア

    st = WriteData(RESERVED_COMMAND_GET_INFO);

    ASerialDataStruct::ASerialData data_buf;

    clock_t read_time = clock();

    while (1) {
        st = ReadDataProcess(&data_buf);

        if (st != 0 || clock() - read_time >= 50) {
            break;
        }
    }

    if (clock() - read_time >= 50 || st == -1 || data_buf.data[0] != GetID() ||
        (data_buf.data[1] < m_device_ver_min && data_buf.data[1] > m_device_ver_max)) {
        m_inteface->ClosePort();
        return ConnectResult::Fail;
    }

    SetConnectionState(true);
    m_com = COM_num;

    return ConnectResult::Succ;
}

ConnectResult UASerialLibControllerWin::AutoConnectDevice() {
    if (GetConnectionState() == true) {
        return ConnectResult::Fail;
    }

    // 2025.08.06 ウー start
    //int ret = 0;
    ConnectResult ret = ConnectResult::Fail;
    // 2025.08.06 end

    for (int i = 1; i <= 255; ++i) {
        int st = ConnectDevice(i);

        if (st == 0) {
            // 2025.08.06 ウー start
            //ret = i;
            m_com = i;
            ret = ConnectResult::Succ;
            // 2025.08.06 end
            break;
        }
    }
    // 2025.08.06 ウー start
    //if (ret == 0) {
    //    return ConnectResult::Fail;
    //}
    // 2025.08.06 end
    return ret;
}

ConnectResult UASerialLibControllerWin::DisConnectDevice() {
    if (m_inteface->GetState() == false) {
        return ConnectResult::Fail;
    }

    int st = m_inteface->ClosePort();

    SetConnectionState(false);

    // 2025.08.06 ウー start
    //return st;
    if (st == 0)
        return ConnectResult::Succ;
    else
        return ConnectResult::Fail;
    // 2025.08.06 end
}

void UASerialLibControllerWin::SetInterfacePt(WindowsSerial* interface_pt) { m_inteface = interface_pt; }

int UASerialLibControllerWin::ReadDataProcess(ASerialDataStruct::ASerialData* read_data_buf)
{
    if (m_inteface->GetState() == false) {
        return -1;
    }

    int st = 0;
    if (m_inteface->available() > 0) {
        uint8_t read_c = (uint8_t)m_inteface->read();
        st = ReadPacketData(read_c, read_data_buf);
    }

    if (st == -1) {
        return -1;
    }

    return st;
}

int UASerialLibControllerWin::ReadData(ASerialDataStruct::ASerialData* read_data_buf) {
    if (m_inteface->GetState() == false) {
        return -1;
    }

    constexpr clock_t time_out = 50;

    bool time_out_flag = false;
    bool error_flag = false;

    clock_t time_buf = clock();
    while (1) {
        int st = ReadDataProcess(read_data_buf);

        if (st == 1) {
            break;
        }
        else if (st == -1) {
            error_flag = true;
            break;
        }

        if (clock() - time_buf >= time_out) {
            time_out_flag = true;
            break;
        }
    }

    if (error_flag == true) {    //読み取りエラー
        return -1;
    }
    else if (time_out_flag == true) { //タイムアウトエラー
        return -2;
    }

    return 0;
}

int UASerialLibControllerWin::WriteData(uint8_t command, uint8_t* data, uint8_t data_num)
{
    int BUF_SIZE = GetNeedPacketBufSize(data, data_num);

    uint8_t* packet_buf = new uint8_t[BUF_SIZE];

    int st = MakePacketData(data, data_num, command, packet_buf);

    if (st == -1) {
        delete[] packet_buf;
        return -1;
    }

    int write_size = m_inteface->write(packet_buf, BUF_SIZE);

    delete[] packet_buf;

    if (write_size != BUF_SIZE) {
        return -1;
    }

    return 0;
}

int UASerialLibControllerWin::WriteData(uint8_t command)
{
    const int BUF_SIZE = 6;

    uint8_t packet_buf[BUF_SIZE] = { 0 };

    int st = MakePacketData(command, packet_buf);

    if (st == -1) {
        return -1;
    }

    int write_size = m_inteface->write(packet_buf, BUF_SIZE);

    if (write_size != BUF_SIZE) {
        return -1;
    }

    return 0;
}
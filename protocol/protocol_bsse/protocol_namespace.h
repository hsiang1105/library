#ifndef PROTOCOL_NAMESPACE_H
#define PROTOCOL_NAMESPACE_H

#include <stdint.h>

#define _UNUSED(x)      (void)x;

#define BYTE_SIZE       1
#define WORD_SIZE       2
#define DWORD_SIZE      4
#define QWORD_SIZE      8

#define ONE_BIT         1
#define BYTE_BITS       8
#define WORD_BITS       16
#define DWORD_BITS      32
#define QWORD_BITS      64

#define BINARY_BASE     2
#define OCTAL_BASE      8
#define DECIMAL_BASE    10
#define HEX_BASE        16

namespace CommNamespace {
enum Endianness{
    BE,
    LE
};

enum RegType {
    BIT_REG,
    BYTE_REG,
    WORD_REG,
    DWORD_REG,
    QWORD_REG
};

enum ControlChar {
    ASCII_NUL   = 0x00,
    ASCII_SOH   = 0x01,
    ASCII_STX   = 0x02,
    ASCII_ETX   = 0x03,
    ASCII_EOT   = 0x04,
    ASCII_ENQ   = 0x05,
    ASCII_ACK   = 0x06,
    ASCII_LF    = 0x0A,
    ASCII_CR    = 0x0D,
    ASCII_DEL   = 0x10,
    ASCII_NAK   = 0x15,
    ASCII_SYN   = 0x16,
    ASCII_ESC   = 0x1B
};

enum FunctionCode
{
    // PLC
    // basic function
    FUNC_BATCH_GET_VALUES,
    FUNC_BATCH_SET_VALUES,
    FUNC_RANDOM_GET_VALUES,
    FUNC_RANDOM_SET_VALUES,
    FUNC_BATCH_GET_DISCRETE_STATUS,
    FUNC_BATCH_SET_DISCRETE_STATUS,

    // extension function
    FUNC_GET_SYSTEM_BRIEF,
    FUNC_RUN_DEVICE,
    FUNC_STOP_DEVICE,
    FUNC_TESTING,
    FUNC_GET_SYSTEM_STATUS,
    FUNC_LOG_ON,
    FUNC_LOG_OUT,
    FUNC_GET_SYSTEM_VERSION,
    FUNC_GET_STATION_NUMBER,
    FUNC_SET_STATION_NUMBER,
    FUNC_GET_SERIAL_NUMBER,
    FUNC_SET_SERIAL_NUMBER,
    FUNC_GET_PASSWORD,
    FUNC_SET_PASSWORD,
    FUNC_GET_IO_STATUS,
    FUNC_READ_MEMORY,
    FUNC_WRITE_MEMORY,
    FUNC_BLOCK_READ_MEMORY,
    FUNC_BLOCK_WRITE_MEMORY,
    FUNC_EDIT_PROGRAM,
    FUNC_CLEAR_PROGRAM,
    FUNC_CLEAR_REGISTERS,
    FUNC_CLEAR_DISCRETES,
    FUNC_INITIALIZE,
    FUNC_BACKUP,
    FUNC_FIRMWARE_UPDATE,
    FUNC_SET_DEBUG_INFO,
    FUNC_BLOCK_READ_EX_MEMORY,
    FUNC_BLOCK_WRITE_EX_MEMORY,
    FUNC_GET_MAC_ADDRESS,
    FUNC_AUTHENTICATE_DEVICE,
    FUNC_HANDSHAKE_DEVICE,
    FUNC_DATA_BYPASS,
    FUNC_ECHO_TEST,
    FUNC_CLEAR_PROGRAM_ALL,
    FUNC_CLEAR_PROGRAM_MOT,

    // PLC bypass function
    FUNC_SYNC_EXP_CRC,
    FUNC_INDICATE_POS,
    FUNC_GET_HWID,
    FUNC_GET_CARD_INFO,
    FUNC_SET_CARD_FORMAT,
    FUNC_SET_CARD_PLC_FWUPDATE,
    FUNC_SET_CARD_PLC_RESCUE,
    FUNC_SET_CARD_EXP_FWUPDATE,
    FUNC_SET_CARD_EXP_RESCUE,
    FUNC_SET_CARD_PROTECT,
    FUNC_CLEAR_CARD_ALL,
    FUNC_CLEAR_CARD_DATA_RECORD,
    FUNC_CLEAR_CARD_PLC,
    FUNC_CLEAR_CARD_EXP,
    FUNC_GET_RECIPE_INFO,
    FUNC_GET_RECIPE_DATA,
    FUNC_SET_RECIPE_DATA,
    FUNC_GET_MOT_MAPPING_INFO,
    FUNC_GET_MOT_MAPPING_TABLE,
    FUNC_SET_MOT_MAPPING_TABLE,
    FUNC_GET_SYSLOG,

    // Motion
    FUNC_MOT_READ_PROGRAM_COUNT,
    FUNC_MOT_READ_PROGRAM_SIZE,
    FUNC_MOT_BLOCK_READ_PROGRAM,
    FUNC_MOT_WRITE_COMMAND,
    FUNC_MOT_READ_COMMAND,
    FUNC_MOT_DATA_BYPASS,
    FUNC_MOT_FIRMWARE_UPDATE,
    FUNC_MOT_MONITOR_SETTING,
    FUNC_MOT_MONITOR_START,
    FUNC_MOT_MONITOR_STOP,
    FUNC_MOT_MONITOR_READ,

    // Expansion
    FUNC_EXP_GET_COUNT,
    FUNC_EXP_GET_INFO,
    FUNC_EXP_GET_COFIG,
    FUNC_EXP_SET_COFIG,
    FUNC_EXP_CALIBRATION,
    FUNC_EXP_FIRMWARE_UPDATE,
    FUNC_EXP_CHANGE_MODE,

    // Coupler
    FUNC_COUP_LOG_ON,
    FUNC_COUP_GET_SYSTEM_BRIEF,
    FUNC_COUP_MEM_READ,
    FUNC_COUP_MEM_WRITE,
    FUNC_COUP_BATCH_GET_VALUES,
    FUNC_COUP_BATCH_SET_VALUES,
    FUNC_COUP_BATCH_SET_DISCRETE_VALUES,
    FUNC_COUP_LOG_OUT,
    FUNC_COUP_FIRMWARE_UPDATE,
    FUNC_COUP_BACKUP_EXPANSION,
    FUNC_COUP_GET_MAC_ADDRESS,
    FUNC_COUP_AUTHENTICATE_DEVICE,
    FUNC_COUP_HANDSHAKE_DEVICE,
    FUNC_COUP_DATA_BYPASS,
    FUNC_COUP_GET_HWID,
    FUNC_COUP_SYNC_EXP_CRC
};

enum CommErrorCode {
    No_Error                    = 0x00000000,   // No error
    Comm_Unit_Invalid           = 0x00001001,   // CommManager Error Code
    ComPort_Error               = 0x10010001,   // ComPort error
    ComPort_Open_Fail           = 0x10010002,   // ComPort open fail
    ComPort_Send_Fail           = 0x10010003,   // ComPort send fail
    ComPort_Recv_Fail           = 0x10010004,   // ComPort receive fail
    Socket_Null                 = 0x10020001,   // Socket is null
    Socket_Connect_Fail         = 0x10020002,   // Socket connect fail
    Socket_Invalid_IP           = 0x10020003,   // Invalid IP
    Socket_Send_Fail            = 0x10020004,   // Socket send fail
    Socket_Recv_Fail            = 0x10020005,   // Socket send fail
    Socket_Unknown              = 0x1002FFFF,   // The error cannnot be identified
    Protocol_Invalid_Head		= 0x20010001,   // Protocol response header is invalid
    Protocol_Invalid_End		= 0x20010002,   // Protocol response header is invalid
    Protocol_Invalid_Length     = 0x20010003,   // Protocol response length is invalid
    Protocol_Invalid_Data		= 0x20010004,   // Protocol response data is invalid, which causes the convertion failed
    Protocol_Invalid_ErrorCheck	= 0x20010005,   // Protocol response checksum/CRC/LRC is invalid
    Protocol_Invalid_Param		= 0x20010006,   // The parameter is invalid
    Protocol_Invalid_Password	= 0x20010007,   // The password is invalid
    Protocol_Invalid_Serial     = 0x20010003,   // Serial number of received protocol packet is different from the sent one
    Protocol_Exception          = 0x200A0000,   // The protocol response with exception, this is the base
    Command_Timeout             = 0x400A0000,   // Command timeout
    Command_Send_Failed         = 0x400A0001,   // Command send timeout
    Command_Recv_Failed         = 0x400A0002,   // Command recive timeout
    Command_Nack                = 0x400B0001,   // Command returning nack
    Command_Unknown             = 0x400B0002,   // Command returning unknown or invalid expected data
    Command_Not_Support         = 0x400B0003,   // Command performed is not supported
    API_Parameter_Error			= 0x400C0001,   // Parameter error
    Internal_Error			    = 0x400CFFFF,   // Internal error
    External_Error			    = 0x80000000    // External error
};

uint16_t CalculateCRC16(const char *start, int offset, int len);
}

#endif // PROTOCOL_NAMESPACE_H

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <vector>

#define _UNUSED(x) (void)x;

// data bytes
#define BYTE_SIZE 1
#define WORD_SIZE 2
#define DWORD_SIZE 4
#define QWORD_SIZE 8

// databits
#define ONE_BIT 1
#define BYTE_BITS 8
#define WORD_BITS 16
#define DWORD_BITS 32
#define QWORD_BITS 64

// number base
#define BIN_BASE 2  // binary
#define OCT_BASE 8  // octal
#define DEC_BASE 10  // decimal
#define HEX_BASE 16  // hexadecimal

#ifndef VecU8
using VecU8 = std::vector<uint8_t>;
#endif

namespace Protocol {
enum ByteOrder
{
    // big endian
    E_BE,
    // little endian
    E_LE,
};

enum ControlCode
{
    // null
    E_NUL = 0x00,
    // start of heading
    E_SOH = 0x01,
    // start of text
    E_STX = 0x02,
    // end of text
    E_ETX = 0x03,
    // end of transmission
    E_EOT = 0x04,
    // enquiry
    E_ENQ = 0x05,
    // acknowledgement
    E_ACK = 0x06,
    // bell \a
    E_BEL = 0x07,
    // backspace \b
    E_BS = 0x08,
    // horizontal tab \t
    E_HT = 0x09,
    // line feed \n
    E_LF = 0x0A,
    // vertical tab \v
    E_VT = 0x0B,
    // form feed \f
    E_FF = 0x0C,
    // carriage return \r
    E_CR = 0x0D,
    // shift out
    E_SO = 0x0E,
    // shift in
    E_SI = 0x0F,
    // data link escape
    E_DLE = 0x10,
    // negative acknowledgement
    E_NAK = 0x15,
    // synchronous idle
    E_SYN = 0x16,
    // escape \e
    E_ESC = 0x1B,
    // delete
    E_DEL = 0x7F
};

enum FunctionCode
{
    // r/w smae type and continuous address, ex: Y0~Y99
    E_FUNC_BATCH_READ_VALUES,
    E_FUNC_BATCH_WRITE_VALUES,

    // r/w same type and random address, ex: Y0, Y45, Y123
    E_FUNC_RAMDOM_READ_VALUES,
    E_FUNC_RAMDOM_WRITE_VALUES,

    // r/w mix type and address, ex: Y0, D45, R123
    E_FUNC_MIX_READ_VALUES,
    E_FUNC_MIX_WRITE_VALUES,

    // r/w enable state of elements
    E_FUNC_BATCH_READ_ENABLE_STATE,
    E_FUNC_BATCH_WRITE_ENABLE_STATE,

    // monitor element
    E_FUNC_REGISTER_MONITOR,
    E_FUNC_READ_MONITOR,

    // read information
    E_FUNC_READ_BRIEF_INFO,
    E_FUNC_READ_DETAILED_INFO,

    // read status
    E_FUNC_READ_BRIEF_STATUS,
    E_FUNC_READ_DETAILED_STATUS,

    // start/end communication
    E_FUNC_START_TRANSACTION,
    E_FUNC_END_TRANSACTION,

    // run/stop device
    E_FUNC_CONTROL_RUN,
    E_FUNC_CONTROL_STOP,

    // clear devuce
    E_FUNC_REMOTE_CLEAR,

    // loopback test
    E_FUNC_LOOPBACK,

    // user bypass data
    E_FUNC_USER_BYPASS,
};

enum ErrorCode
{
    // no error
    E_NO_ERROR = 0x00000000,
    // property error
    E_PROPERTY_ERROR = 0x20010001,
    // property function error
    E_FUNCTION_ERROR = 0x20010002,
    // property fragment error
    E_FRAGMENT_ERROR = 0x20010003,
    // invalid message header
    E_INVALID_HEADER = 0x20020001,
    // invalid message trailer
    E_INVALID_TRAILER = 0x20020002,
    // invalid message command
    E_INVALID_COMMAND = 0x20020003,
    // invalid data address
    E_INVALID_ADDRESS = 0x20020004,
    // invalid data count
    E_INVALID_COUNT = 0x20020005,
    // invalid data value
    E_INVALID_VALUE = 0x20020006,
    // invalid error check value
    E_INVALID_ERROR_CHECK = 0x20020007,
    // invalid message parameter
    E_INVALID_PARAMETER = 0x20020008,
    // invalid message length
    E_INVALID_MESSAGE_LENGTH = 0x20020009,
    // protocol exception code base
    // return (E_EXCEPTION_ERROR | excepttion code)
    E_EXCEPTION_ERROR = 0x20040000,
    // user error base
    // return (E_USER_ERROR | user defined error)
    E_USER_ERROR = 0x20080000,
};

enum Interface
{
    E_NO_INTERFACE,
    E_SERIAL_PORT,
    E_ETHERNET,
    E_SIMULATOR = 0xFF
};

enum ParameterType
{
    E_PROTOCOL_VERSION,
    E_INTERFACE,
    E_FRAGMENT_SIZE,
    E_USER_PARAMETER = 0x1000
};

struct ReturnData
{
    ReturnData();
    VecU8 data;
    int error;
};

uint8_t Ascii2Hex(uint8_t ascii);
uint8_t Hex2Ascii(uint8_t hex);
VecU8 Value2Data(uint32_t value, int size, ByteOrder endian, bool ascii_flag);
uint32_t Data2Value(const VecU8 &data,
                    int size,
                    ByteOrder endian,
                    bool ascii_flag);

void AppendValue(VecU8 &data,
                 uint32_t value,
                 int size,
                 ByteOrder endian,
                 bool ascii_flag);

uint16_t Checksum(const char *start, int offset, int len);
uint16_t CRC16(const char *start, int offset, int len);
}  // namespace Protocol

#endif  // PROTOCOL_H

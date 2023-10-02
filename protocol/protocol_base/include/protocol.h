#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <vector>

#define _UNUSED(x)      (void)x;

// data bytes
#define BYTE_SIZE       1
#define WORD_SIZE       2
#define DWORD_SIZE      4
#define QWORD_SIZE      8

// databits
#define ONE_BIT         1
#define BYTE_BITS       8
#define WORD_BITS       16
#define DWORD_BITS      32
#define QWORD_BITS      64

// number base
#define BIN_BASE    2   // binary
#define OCT_BASE    8   // octal
#define DEC_BASE    10  // decimal
#define HEX_BASE    16  // hexadecimal

#ifndef ByteArray
using ByteArray = std::vector<uint8_t>;
#endif

namespace Protocol {
enum ByteOrder
{
    E_BE,   // big endian
    E_LE,   // little endian
};

enum ControlCode
{
    E_NUL = 0x00,   // null
    E_SOH = 0x01,   // start of heading
    E_STX = 0x02,   // start of text
    E_ETX = 0x03,   // end of text
    E_EOT = 0x04,   // end of transmission
    E_ENQ = 0x05,   // enquiry
    E_ACK = 0x06,   // acknowledgement
    E_BEL = 0x07,   // bell \a
    E_BS = 0x08,    // backspace \b
    E_HT = 0x09,    // horizontal tab \t
    E_LF = 0x0A,    // line feed \n
    E_VT = 0x0B,    // vertical tab \v
    E_FF = 0x0C,    // form feed \f
    E_CR = 0x0D,    // carriage return \r
    E_SO = 0x0E,    // shift out
    E_SI = 0x0F,    // shift in
    E_DLE = 0x10,   // data link escape
    E_NAK = 0x15,   // negative acknowledgement
    E_SYN = 0x16,   // synchronous idle
    E_ESC = 0x1B,   // escape \e
    E_DEL = 0x7F    // delete
};

enum FunctionCode
{
    // bit device use
    E_FUNC_BATCH_SET,
    E_FUNC_RANDOM_SET,
    E_FUNC_BATCH_RESET,
    E_FUNC_RANDOM_RESET,

    // 8/16/32 bits device use
    E_FUNC_BATCH_READ_VALUES,
    E_FUNC_BATCH_WRITE_VALUES,
    E_FUNC_RAMDOM_READ_VALUES,
    E_FUNC_RAMDOM_WRITE_VALUES,

    // 1/8/16/32 bits device use
    E_FUNC_MIX_READ_VALUES,
    E_FUNC_MIX_WRITE_VALUES,

    // 1/8/16/32 bits device enable state
    E_FUNC_BATCH_READ_ENABLE_STATE,
    E_FUNC_BATCH_WRITE_ENABLE_STATE,

    // monitor device
    E_FUNC_REGISTER_MONITOR,
    E_FUNC_READ_MONITOR,

    // control
    E_FUNC_READ_BRIEF_INFO,
    E_FUNC_READ_DETAILED_INFO,
    E_FUNC_READ_BRIEF_STATUS,
    E_FUNC_READ_DETAILED_STATUS,
    E_FUNC_START_TRANSACTION,
    E_FUNC_END_TRANSACTION,
    E_FUNC_CONTROL_RUN,
    E_FUNC_CONTROL_STOP,
    E_FUNC_REMOTE_CLEAR,
    E_FUNC_LOOPBACK,
    E_FUNC_USER_BYPASS,
};

enum ErrorCode
{
    E_NO_ERROR,                     // no error
    E_HEADER_ERROR,                 // protocol header error
    E_TRAILER_ERROR,                // protocol trailer error
    E_FUNCTION_ERROR,               // protocol function error
    E_INVALID_ADDRESS,              // invalid data address
    E_INVALID_VALUE,                // invalid data value
    E_INVALID_LENGTH,               // invalid data length
    E_INVALID_ERROR_CHECK,          // invalid error check value
    // protocol exception code base (E_EXCEPTION_ERROR | excepttion code)
    E_EXCEPTION_ERROR = 0xE0000000,
};

enum Interface
{
    E_NO_INTERFACE,
    E_SERIAL,
    E_ETHERNET,
    E_USB,
    E_SIMULATION,
    E_CLOUD,
    E_USER_INTERFACE = 0x1000
};

enum ParameterType
{
    E_PROTOCOL_VERSION,
    E_INTERFACE,
    E_FRAGMENT_SIZE,
    E_USER_PARAMETER = 0x1000
};

inline uint8_t Ascii2Hex(uint8_t ascii);
inline uint8_t Hex2Ascii(uint8_t hex);
ByteArray Value2Array(uint32_t value,
                      int size,
                      ByteOrder endian,
                      bool ascii_flag);

uint32_t Array2Value(const ByteArray &array,
                     int size,
                     ByteOrder endian,
                     bool ascii_flag);

uint16_t Checksum(const char *start, int offset, int len);
uint16_t CRC16(const char *start, int offset, int len);
}

#endif  // PROTOCOL_H

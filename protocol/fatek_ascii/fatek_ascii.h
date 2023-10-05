#ifndef FATEK_ASCII_H
#define FATEK_ASCII_H

namespace FatekAscii {
enum ErrorCode {
    E_ERROR_FREE                = 0x30,     // Error free
    E_ILLEGAL_VALUE             = 0x32,     // Illegal value
    E_ILLEGAL_FORMAT            = 0x34,     // Illegal format, or communication command can not execute
    E_CHKSUM_ERROR              = 0x35,     // Can not run(Ladder Checksum error when run PLC)
    E_ID_ERROR                  = 0x36,     // Can not run(PLC ID≠Ladder ID when run PLC)
    E_SYNTAX_ERROR              = 0x37,     // Can not run(Syntax check error when run PLC)
    E_FUNCTION_NOT_SUPPORT      = 0x39,     // Can not run(Function not supported)
    E_ILLEGAL_ADDRRESS          = 0x3A,     // Illegal address
};

enum CommandCode {
    E_CMD_READ_GIST_STATUS          = 0x40,     // The gist read the system status of PLC
    E_CMD_CONTROL_RUN               = 0x41,     // Control RUN/STOP of PLC
    E_CMD_CONTROL_SINGLE_DISC       = 0x42,     // Single discrete control
    E_CMD_READ_CONT_DISC_STATUS     = 0x43,     // The status reading of ENABLE/DISABLE of continuous discrete
    E_CMD_READ_CONT_DISC            = 0x44,     // The status reading of continuous discrete
    E_CMD_WRITE_CONT_DISC           = 0x45,     // Write the status to continuous discrete
    E_CMD_READ_CONT_REG             = 0x46,     // Read the data from continuous registers
    E_CMD_WRITE_CONT_REG            = 0x47,     // Write to continuous registers
    E_CMD_READ_RANDOM               = 0x48,     // Mixed read the random discrete status of register data
    E_CMD_WRITE_RANDOM              = 0x49,     // Mixed write the random discrete status of register data
    E_CMD_LOOKBACK_TEST             = 0x4E,     // Loop back testing
    E_CMD_READ_DETAIL_STATUS        = 0x53      // The detail read the system status of PLC
};

enum ControlRun
{
    E_STOP,
    E_RUN
};

enum ControlDiscrete
{
    E_CONTROL_DISABLE = 1,
    E_CONTROL_ENABLE = 2,
    E_CONTROL_SET = 3,
    E_CONTROL_RESET = 4
};

constexpr int kErrorIndex = 5;          // error code index of reply message
constexpr int kRequestHeaderLen = 5;    // STX(1), STN(2), CMD(2)
constexpr int kRequestTrailerLen = 3;   // CSUM(2), ETX(1)
constexpr int kResponseHeaderLen = 6;   // STX(1), STN(2), CMD(2), ERR(1)
constexpr int kResponseTrailerLen = 3;  // CSUM(2), ETX(1)
constexpr int kGistStatusLen = 6;       // STS1(2), STS2(2), STS3(2)
constexpr int kDetailtStatusLen = 128;  // STS1(2), ... STS64(2)
constexpr int kFragmentLen = 256;
constexpr int kReadRandomPoints = 64;
constexpr int kWriteRandomPoints = 32;

}  // namespace FatekAscii


#endif // FATEK_ASCII_H

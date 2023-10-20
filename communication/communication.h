#ifndef COMMUNICATION_H
#define COMMUNICATION_H

namespace Communication {
enum ErrorCode
{
    // no error
    E_NO_ERROR = 0x00000000,
    // client interface error
    E_CLIENT_INTERFACE_ERROR = 0x10000001,
    // client open failed
    E_CLIENT_OPEN_FAILED = 0x10000002,
    // client reopen failed
    E_CLIENT_REOPEN_FAILED = 0x10000003,
    // client send failed
    E_CLIENT_SEND_FAILED = 0x10000004,
    // client receive failed
    E_CLIENT_RECV_FAILED = 0x10000005,
    // client buffer error
    E_CLIENT_BUFFER_ERROR = 0x10000006,
    // client timeout
    E_CLIENT_TIMEOUT = 0x10000007,
    // invaliid protocol
    E_INVALID_PROTOCOL = 0x20000001,
    // invalid request
    E_INVALID_REQUEST = 0x40000001,
    // internal error
    E_INTERNAL_ERROR = 0x40010001,
    // external error
    E_EXTERNAL_ERROR = 0x40020001,
};
}

#endif  // COMMUNICATION_H

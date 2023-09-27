#ifndef ABSTRACT_MESSAGE_H
#define ABSTRACT_MESSAGE_H

#include "protocol_property.h"

namespace Protocol {

class AbstractMessage
{
public:
    AbstractMessage();
    virtual ~AbstractMessage();

    virtual bool SetProperty(const Property &prop);
    virtual bool SetResponse(const ByteArray &response);
    virtual bool IsTerminated(const ByteArray &response);

    int Error() const { return error_; }
    ByteArray Request() const { return request_; }
    ByteArray Result() const { return result_; }
    int ExpectedResponseLength() const { return expected_resp_len_; }

protected:
    virtual bool composeHeader() = 0;
    virtual bool composePayload() = 0;
    virtual bool composeTrailer() = 0;

    virtual bool verifyFormat() = 0;
    virtual bool checkException() = 0;
    virtual bool decomposePayload() = 0;

protected:
    int error_;
    int expected_resp_len_;
    Property prop_;
    ByteArray request_;
    ByteArray response_;
    ByteArray result_;
};

}  // namespace Protocol

#endif  // ABSTRACT_MESSAGE_H

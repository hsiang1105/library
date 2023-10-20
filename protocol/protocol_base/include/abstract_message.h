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
    virtual bool SetResponse(const VecU8 &response);
    virtual bool IsTerminated(const VecU8 &response);

    int Error() const { return error_; }
    int ExpectedResponseLength() const { return expected_resp_len_; }
    VecU8 Request() const { return request_; }
    VecU8 Extraction() const { return extraction_; }

protected:
    virtual bool composeHeader() = 0;
    virtual bool composePayload() = 0;
    virtual bool composeTrailer() = 0;

    virtual bool verifyFormat() = 0;
    virtual bool checkException() = 0;
    virtual bool extractPayload() = 0;

protected:
    int error_;
    int expected_resp_len_;
    Property prop_;
    VecU8 request_;
    VecU8 response_;
    VecU8 extraction_;
};

}  // namespace Protocol

#endif  // ABSTRACT_MESSAGE_H

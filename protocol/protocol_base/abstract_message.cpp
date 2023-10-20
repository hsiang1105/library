#include "abstract_message.h"

namespace Protocol {

AbstractMessage::AbstractMessage()
  : error_(E_NO_ERROR)
  , expected_resp_len_(0)
{
}

AbstractMessage::~AbstractMessage() {}

bool AbstractMessage::SetProperty(const Property &prop)
{
    error_ = E_NO_ERROR;
    expected_resp_len_ = 0;
    request_.clear();
    prop_ = prop;

    if (composeHeader()) {
        if (composePayload()) {
            if (composeTrailer()) {
                return true;
            }
        }
    }

    return false;
}

bool AbstractMessage::SetResponse(const VecU8 &response)
{
    error_ = E_NO_ERROR;
    extraction_.clear();
    response_ = response;

    if (verifyFormat()) {
        if (checkException()) {
            if (extractPayload()) {
                return true;
            }
        }
    }

    return false;
}

bool AbstractMessage::IsTerminated(const VecU8 &response)
{
    _UNUSED(response)
    return false;
}

}  // namespace Protocol

#ifndef FBS_PROTOCOL_H
#define FBS_PROTOCOL_H

#include "abstract_protocol.h"

namespace Protocol {
class FatekAsciiMessage;
class FatekAsciiProtocol : public AbstractProtocol
{
public:
    FatekAsciiProtocol();
    ~FatekAsciiProtocol();

    Result GenerateRequest(const Property &prop) override;
    Result HandleResponse(const Property &prop,
                          const ByteArray &response) override;

    int ExpectedResponseLength(const Property &prop) override;
    bool IsTerminated(const ByteArray &response) override;
    PropertyList Fragment(const Property &prop) override;

private:
    PropertyList fragmentBatchRead(const Property &prop);
    PropertyList fragmentBatchWrite(const Property &prop);
    PropertyList fragmentRandomRead(const Property &prop);
    PropertyList fragmentRandomWrite(const Property &prop);

private:
    std::map<int, FatekAsciiMessage *> message_map_;
};
}  // namespace Protocol

#endif  // FBS_PROTOCOL_H

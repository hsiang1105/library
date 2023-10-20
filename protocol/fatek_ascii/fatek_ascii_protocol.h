#ifndef FATEK_ASCII_PROTOCOL_H
#define FATEK_ASCII_PROTOCOL_H

#include "abstract_protocol.h"

namespace Protocol {
class FatekAsciiMessage;
class FatekAsciiProtocol : public AbstractProtocol
{
public:
    FatekAsciiProtocol();
    ~FatekAsciiProtocol();

    ReturnData GenerateRequest(const Property &prop) override;
    ReturnData HandleResponse(const Property &prop,
                              const VecU8 &response) override;

    int ExpectedResponseLength(const Property &prop) override;
    bool IsTerminated(const VecU8 &response) override;
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

#endif  // FATEK_ASCII_PROTOCOL_H

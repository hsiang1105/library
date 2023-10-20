#ifndef FATEK_ASCII_MESSAGE_H
#define FATEK_ASCII_MESSAGE_H

#include "abstract_message.h"

namespace Protocol {

class FatekAsciiMessage : public AbstractMessage
{
public:
    FatekAsciiMessage();
    ~FatekAsciiMessage();

    bool IsTerminated(const VecU8 &response) override;

protected:
    bool composeHeader() override;
    bool composePayload() override;
    bool composeTrailer() override;

    bool verifyFormat() override;
    bool checkException() override;
    bool extractPayload() override;

private:
    std::string getTypeName(const Element &element);
    void appendValue(VecU8 &result, uint32_t value, int size = BYTE_SIZE);
    void appendString(VecU8 &result, const std::string &str);
    void appendAddress(VecU8 &result, int addr, int addr_len);

    bool composeMixRead();
    bool composeMixWrite();
    bool composeBatchRead();
    bool composeBatchWrite();
    bool composeBatchReadState();
    bool composeBatchWriteState();
    bool composeReadBriefStatus();
    bool composeReadDetailStatus();
    bool composeControlRun(bool run);
    bool composeLoopback();

    bool extractRead();
    bool extractWrite();
    bool extractCommon();
    bool extractLoopback();
};
}  // namespace Protocol

#endif  // FATEK_ASCII_MESSAGE_H

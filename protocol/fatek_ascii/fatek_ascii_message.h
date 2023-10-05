#ifndef FATEK_ASCII_MESSAGE_H
#define FATEK_ASCII_MESSAGE_H

#include "abstract_message.h"

namespace Protocol {

class FatekAsciiMessage : public AbstractMessage
{
public:
    FatekAsciiMessage();
    ~FatekAsciiMessage();

    bool IsTerminated(const ByteArray &response) override;

protected:
    bool composeHeader() override;
    bool composePayload() override;
    bool composeTrailer() override;

    bool verifyFormat() override;
    bool checkException() override;
    bool decomposePayload() override;

private:
    std::string getTypeName(const Element &element);
    void appendValue(ByteArray &result, uint32_t value, int size = BYTE_SIZE);
    void appendString(ByteArray &result, const std::string &str);
    void appendAddress(ByteArray &result, int addr, int addr_len);

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

    bool decomposeRead();
    bool decomposeWrite();
    bool decomposeCommon();
    bool decomposeLoopback();
};
}  // namespace Protocol

#endif  // FATEK_ASCII_MESSAGE_H

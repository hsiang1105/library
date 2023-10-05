#include "fatek_ascii_message.h"
#include <cmath>
#include "fatek_ascii.h"

using namespace FatekAscii;
using namespace Protocol;

FatekAsciiMessage::FatekAsciiMessage()
  : AbstractMessage()
{
}

FatekAsciiMessage::~FatekAsciiMessage() {}

bool FatekAsciiMessage::IsTerminated(const ByteArray &response)
{
    if ((response.front() == E_STX) && (response.back() == E_ETX)) {
        return true;
    }

    return false;
}

bool FatekAsciiMessage::composeHeader()
{
    request_.push_back(E_STX);
    return true;
}

bool FatekAsciiMessage::composePayload()
{
    bool res = false;
    switch (prop_.Function()) {
        case E_FUNC_RAMDOM_READ_VALUES:
        case E_FUNC_MIX_READ_VALUES:       
            res = composeMixRead();
            break;

        case E_FUNC_RAMDOM_WRITE_VALUES:
        case E_FUNC_MIX_WRITE_VALUES:
            res = composeMixWrite();
            break;

        case E_FUNC_BATCH_READ_VALUES:
            res = composeBatchRead();
            break;

        case E_FUNC_BATCH_WRITE_VALUES:
            res = composeBatchWrite();
            break;

        case E_FUNC_BATCH_READ_ENABLE_STATE:
            res = composeBatchReadState();
            break;

        case E_FUNC_BATCH_WRITE_ENABLE_STATE:
            res = composeBatchWriteState();
            break;

        case E_FUNC_READ_BRIEF_STATUS:
            res = composeReadBriefStatus();
            break;
        case E_FUNC_CONTROL_RUN:
            res = composeControlRun(true);
            break;

        case E_FUNC_CONTROL_STOP:
            res = composeControlRun(false);
            break;

        case E_FUNC_READ_DETAILED_STATUS:
            res = composeReadDetailStatus();
            break;

        case E_FUNC_LOOPBACK:
            res = composeLoopback();
            break;

        default:
            error_ = E_FUNCTION_ERROR;
            return false;
            break;
    }

    return res;
}

bool FatekAsciiMessage::composeTrailer()
{
    uint8_t csum =
        Checksum(reinterpret_cast<char *>(request_.data()), 0, request_.size());

    ByteArray array = Value2Array(csum, BYTE_SIZE, E_BE, true);
    request_.insert(request_.end(), array.begin(), array.end());
    request_.push_back(E_ETX);
    return true;
}

bool FatekAsciiMessage::verifyFormat()
{
    int req_len = request_.size();
    int resp_len = response_.size();

    // check request length
    if (req_len < (kRequestHeaderLen + kRequestTrailerLen)) {
        error_ = E_INVALID_MESSAGE_LENGTH;
        return false;
    }

    // check response length
    if (prop_.Function() == E_FUNC_LOOPBACK) {
        // STX(1), STN(2), CMD(2), CSUM(2), ETX(1)
        if (resp_len < (kRequestHeaderLen + kRequestTrailerLen)) {
            error_ = E_INVALID_MESSAGE_LENGTH;
            return false;
        }

    } else {
        // STX(1), STN(2), CMD(2), ERR(1), CSUM(2), ETX(1)
        if (resp_len < (kResponseHeaderLen + kResponseTrailerLen)) {
            error_ = E_INVALID_MESSAGE_LENGTH;
            return false;
        }
    }

    // check header
    for (int i = 0; i < kRequestHeaderLen; i++) {
        if (response_[i] != request_[i]) {
            error_ = E_INVALID_HEADER;
            return false;
        }
    }

    // check checksum
    char *start = reinterpret_cast<char *>(response_.data());
    int cal_offset = 0;
    int cal_len = resp_len - kResponseTrailerLen;
    uint8_t cal_csum = Checksum(start, cal_offset, cal_len) & 0xFF;
    ByteArray array(response_.begin() + cal_offset,
                    response_.begin() + cal_offset + cal_len);

    uint8_t csum = Array2Value(array, BYTE_SIZE, E_BE, true);
    if (csum != cal_csum) {
        error_ = E_INVALID_ERROR_CHECK;
        return false;
    }

    // check trailer
    if (response_.back() != E_ETX) {
        error_ = E_INVALID_TRAILER;
        return false;
    }

    return true;
}

bool FatekAsciiMessage::checkException()
{
    if (prop_.Function() == E_FUNC_LOOPBACK) {
        return true;
    }

    int resp_len = response_.size();
    if (resp_len < kResponseHeaderLen) {
        error_ = E_INVALID_MESSAGE_LENGTH;
        return false;
    }

    // check error code
    if (response_.at(kErrorIndex) != E_ERROR_FREE) {
        uint8_t err_code = Ascii2Hex(response_.at(kErrorIndex));
        error_ = E_EXCEPTION_ERROR + err_code;
        return false;
    }

    return true;
}

bool FatekAsciiMessage::decomposePayload()
{
    bool res = false;
    switch (prop_.Function()) {
        case E_FUNC_MIX_READ_VALUES:
        case E_FUNC_BATCH_READ_VALUES:
        case E_FUNC_BATCH_READ_ENABLE_STATE:
            res = decomposeRead();
            break;

        case E_FUNC_MIX_WRITE_VALUES:
        case E_FUNC_BATCH_WRITE_VALUES:
        case E_FUNC_BATCH_WRITE_ENABLE_STATE:
            res = decomposeWrite();
            break;

        case E_FUNC_READ_BRIEF_STATUS:
        case E_FUNC_CONTROL_RUN:
        case E_FUNC_CONTROL_STOP:
        case E_FUNC_READ_DETAILED_STATUS:
            res = decomposeCommon();
            break;

        case E_FUNC_LOOPBACK:
            res = decomposeLoopback();
            break;
        default:
            error_ = E_FUNCTION_ERROR;
            break;
    }

    return res;
}

std::string FatekAsciiMessage::getTypeName(const Element &element)
{
    std::string type = element.Type();
    int databits = element.Databits();
    if (!type.compare("T")) {
        if (databits == WORD_BITS) {
            type = std::string("RT");
        }
    } else if (!type.compare("C")) {
        if (databits == WORD_BITS) {
            type = std::string("RC");
        } else if (databits == DWORD_BITS) {
            type = std::string("DRC");
        }
    }

    return type;
}

void FatekAsciiMessage::appendValue(ByteArray &result, uint32_t value, int size)
{
    AppendValue(result, value, size, E_BE, true);
}

void FatekAsciiMessage::appendString(ByteArray &result, const std::string &str)
{
    int size = str.size();
    for (int i = 0; i < size; i++) {
        char c = str[i];
        c = toupper(c);
        result.push_back(c);
    }
}

void FatekAsciiMessage::appendAddress(ByteArray &result, int addr, int addr_len)
{
    while (addr_len) {
        int order = pow(10, addr_len - 1);
        uint8_t byte = addr / order;
        result.push_back(Hex2Ascii(byte));
        addr = addr % order;
        addr_len--;
    }
}

bool FatekAsciiMessage::composeMixRead()
{
    int total_points = 0;
    int total_bytes = 0;
    for (const Element &element : prop_.Elements()) {
        total_points += element.Count();
        total_bytes += element.DataBytes();
    }

    if (total_points > kReadRandomPoints) {
        error_ = E_INVALID_COUNT;
        return false;
    }

    appendValue(request_, prop_.Station());

    uint8_t cmd = E_CMD_READ_RANDOM;
    appendValue(request_, cmd);

    uint8_t total_number = prop_.Elements().size();
    appendValue(request_, total_number);

    for (const Element &element : prop_.Elements()) {
        int component_len = 5;
        switch (element.Databits()) {
            case ONE_BIT:
                component_len = 5;
                break;

            case WORD_BITS:
                component_len = 6;
                break;

            case DWORD_BITS:
                component_len = 7;
                break;

            default:
                break;
        }

        std::string type = getTypeName(element);
        appendString(request_, type);

        int addr_len = component_len - type.length();
        appendAddress(request_, element.Address(), addr_len);
    }

    expected_resp_len_ =
        kResponseHeaderLen + (total_bytes << 1) + kResponseTrailerLen;

    return true;
}

bool FatekAsciiMessage::composeMixWrite()
{
    int total_points = 0;
    for (const Element &element : prop_.Elements()) {
        total_points += element.Count();
    }

    if (total_points > kWriteRandomPoints) {
        error_ = E_INVALID_COUNT;
        return false;
    }

    appendValue(request_, prop_.Station());

    uint8_t cmd = E_CMD_WRITE_RANDOM;
    appendValue(request_, cmd);

    uint8_t total_number = prop_.Elements().size();
    appendValue(request_, total_number);

    int offset = 0;
    for (const Element &element : prop_.Elements()) {
        std::string type = getTypeName(element);
        ByteArray data = prop_.Data();
        int component_len = 0;
        int addr_len = 0;

        switch (element.Databits()) {
            case ONE_BIT:
                component_len = 5;
                addr_len = component_len - type.length();
                appendString(request_, type);
                appendAddress(request_, element.Address(), addr_len);
                if (data[offset++]) {
                    request_.push_back(Hex2Ascii(0x01));
                } else {
                    request_.push_back(Hex2Ascii(0x00));
                }
                break;

            case WORD_BITS:
                component_len = 6;
                addr_len = component_len - type.length();
                appendString(request_, type);
                appendAddress(request_, element.Address(), addr_len);
                for (int i = 0; i < element.DataBytes(); ++i) {
                    appendValue(request_, data[offset++]);
                }
                break;

            case DWORD_BITS:
                component_len = 7;
                addr_len = component_len - type.length();
                appendString(request_, type);
                appendAddress(request_, element.Address(), addr_len);
                for (int i = 0; i < element.DataBytes(); ++i) {
                    appendValue(request_, data[offset++]);
                }
                break;

            default:
                break;
        }
    }

    expected_resp_len_ = kResponseHeaderLen + kResponseTrailerLen;
    return true;
}

bool FatekAsciiMessage::composeBatchRead()
{
    if (prop_.Elements().size() != 1) {
        error_ = E_INVALID_COUNT;
        return false;
    }

    Element element = prop_.Elements().front();
    if (element.DataBytes() > kFragmentLen) {
        error_ = E_FRAGMENT_ERROR;
        return false;
    }

    int count = element.Count();
    std::string type = getTypeName(element);

    // Multiple Bits(<= 256)
    // STN(2), CMD(2), LEN(2), TYPE(1), ADDR(4)
    if (element.Databits() == ONE_BIT) {
        appendValue(request_, prop_.Station());

        uint8_t cmd = E_CMD_READ_CONT_DISC;
        appendValue(request_, cmd);
        appendValue(request_, count);
        appendString(request_, type);

        int addr_len = 4;
        appendAddress(request_, element.Address(), addr_len);

        expected_resp_len_ = kResponseHeaderLen + count + kResponseTrailerLen;

    }
    // Multiple WORDs
    // STN(2), CMD(2), LEN(2), TYPE + ADDR(6)
    // Multiple DWORDs
    // STN(2), CMD(2), LEN(2), TYPE + ADDR(7)
    else {
        appendValue(request_, prop_.Station());

        uint8_t cmd = E_CMD_READ_CONT_REG;
        appendValue(request_, cmd);
        appendValue(request_, count);
        appendString(request_, type);

        // component length of WORD is 6, DWORD is 7
        // "WX" (2) + addr(4)
        // "R"  (1) + addr(5)
        // "DWX"(3) + addr(4)
        // "DR" (2) + addr(5)
        int component_len = (element.Databits() == DWORD_BITS) ? 7 : 6;
        int addr_len = component_len - type.size();
        appendAddress(request_, element.Address(), addr_len);

        if (element.Databits() == DWORD_BITS) {
            expected_resp_len_ =
                kResponseHeaderLen + (count << 3) + kResponseTrailerLen;
        } else {
            expected_resp_len_ =
                kResponseHeaderLen + (count << 2) + kResponseTrailerLen;
        }
    }

    return true;
}

bool FatekAsciiMessage::composeBatchWrite()
{
    if (prop_.Elements().size() != 1) {
        error_ = E_INVALID_COUNT;
        return false;
    }

    Element element = prop_.Elements().front();
    if (element.DataBytes() > kFragmentLen) {
        error_ = E_FRAGMENT_ERROR;
        return false;
    }

    ByteArray data = prop_.Data();
    std::string type = getTypeName(element);
    int count = element.Count();

    // Single Bit
    // STN(2), CMD(2), CTRL(1), TYPE(1), ADDR(4)
    // Multiple Bits
    // STN(2), CMD(2), LEN(2), TYPE(1), ADDR(4), data
    if (element.Databits() == ONE_BIT) {
        appendValue(request_, prop_.Station());

        int addr_len = 4;
        if (count == 1) {
            uint8_t cmd = E_CMD_CONTROL_SINGLE_DISC;
            appendValue(request_, cmd);

            if (data.at(0) & 0x01) {
                request_.push_back(Hex2Ascii(E_CONTROL_SET));  // set
            } else {
                request_.push_back(Hex2Ascii(E_CONTROL_RESET));  // reset
            }

            appendString(request_, type);
            appendAddress(request_, element.Address(), addr_len);

        } else {
            uint8_t cmd = E_CMD_WRITE_CONT_DISC;
            appendValue(request_, cmd);
            appendValue(request_, count);
            appendString(request_, type);
            appendAddress(request_, element.Address(), addr_len);

            for (uint8_t byte : data) {
                request_.push_back(Hex2Ascii(byte));
            }
        }
    }
    // Multiple WORDs
    // STN(2), CMD(2), LEN(2), TYPE + ADDR(6), data
    // Multiple DWORDs
    // STN(2), CMD(2), LEN(2), TYPE + ADDR(7), data
    else {
        appendValue(request_, prop_.Station());

        uint8_t cmd = E_CMD_WRITE_CONT_REG;
        appendValue(request_, cmd);
        appendValue(request_, count);
        appendString(request_, type);

        // component length of WORD is 6, DWORD is 7
        // "WX" (2) + addr(4)
        // "R"  (1) + addr(5)
        // "DWX"(3) + addr(4)
        // "DR" (2) + addr(5)
        int component_len = (element.Databits() == DWORD_BITS) ? 7 : 6;
        int addr_len = component_len - type.size();
        appendAddress(request_, element.Address(), addr_len);

        for (uint8_t byte : data) {
            appendValue(request_, byte);
        }
    }

    expected_resp_len_ = kResponseHeaderLen + kResponseTrailerLen;
    return true;
}

bool FatekAsciiMessage::composeBatchReadState()
{
    if (prop_.Elements().size() != 1) {
        error_ = E_INVALID_COUNT;
        return false;
    }

    Element element = prop_.Elements().front();
    if (element.Databits() != ONE_BIT) {
        error_ = E_INVALID_PARAMETER;
        return false;
    }

    if (element.DataBytes() > kFragmentLen) {
        error_ = E_FRAGMENT_ERROR;
        return false;
    }

    // Multiple Bits(<= 256)
    // STN(2), CMD(2), LEN(2), TYPE(1), ADDR(4)
    appendValue(request_, prop_.Station());

    uint8_t cmd = E_CMD_READ_CONT_DISC_STATUS;
    appendValue(request_, cmd);

    int count = element.Count();
    appendValue(request_, count);

    std::string type = getTypeName(element);
    appendString(request_, type);

    int addr_len = 4;
    appendAddress(request_, element.Address(), addr_len);

    expected_resp_len_ = kResponseHeaderLen + count + kResponseTrailerLen;

    return true;
}

bool FatekAsciiMessage::composeBatchWriteState()
{
    if (prop_.Elements().size() != 1) {
        error_ = E_INVALID_COUNT;
        return false;
    }

    Element element = prop_.Elements().front();
    if (element.Databits() != ONE_BIT) {
        error_ = E_INVALID_PARAMETER;
        return false;
    }

    if (element.DataBytes() != 1) {
        error_ = E_FRAGMENT_ERROR;
        return false;
    }

    // Single Bit
    // STN(2), CMD(2), CTRL(1), TYPE(1), ADDR(4)
    appendValue(request_, prop_.Station());

    uint8_t cmd = E_CMD_CONTROL_SINGLE_DISC;
    appendValue(request_, cmd);

    ByteArray data = prop_.Data();
    if (data.front()) {
        result_.push_back(Hex2Ascii(E_CONTROL_ENABLE));
    } else {
        result_.push_back(Hex2Ascii(E_CONTROL_DISABLE));
    }

    std::string type = getTypeName(element);
    appendString(request_, type);

    int addr_len = 4;
    appendAddress(request_, element.Address(), addr_len);

    expected_resp_len_ = kResponseHeaderLen + kResponseTrailerLen;

    return true;
}

bool FatekAsciiMessage::composeReadBriefStatus()
{
    appendValue(request_, prop_.Station());

    uint8_t cmd = E_CMD_READ_GIST_STATUS;
    appendValue(request_, cmd);

    expected_resp_len_ =
        kResponseHeaderLen + kGistStatusLen + kResponseTrailerLen;

    return true;
}

bool FatekAsciiMessage::composeReadDetailStatus()
{
    appendValue(request_, prop_.Station());

    uint8_t cmd = E_CMD_READ_DETAIL_STATUS;
    appendValue(request_, cmd);

    expected_resp_len_ =
        kResponseHeaderLen + kDetailtStatusLen + kResponseTrailerLen;

    return true;
}

bool FatekAsciiMessage::composeControlRun(bool run)
{
    appendValue(request_, prop_.Station());

    uint8_t cmd = E_CMD_CONTROL_RUN;
    appendValue(request_, cmd);

    if (run) {
        result_.push_back(Hex2Ascii(E_RUN));
    } else {
        result_.push_back(Hex2Ascii(E_STOP));
    }

    expected_resp_len_ = kResponseHeaderLen + kResponseTrailerLen;

    return true;
}

bool FatekAsciiMessage::composeLoopback()
{
    ByteArray data = prop_.Data();
    if (data.empty()) {
        error_ = E_INVALID_VALUE;
        return false;
    }

    appendValue(request_, prop_.Station());

    uint8_t cmd = E_CMD_LOOKBACK_TEST;
    appendValue(request_, cmd);
    request_.insert(request_.end(), data.begin(), data.end());

    expected_resp_len_ = kRequestHeaderLen + data.size() + kRequestTrailerLen;

    return true;
}

bool FatekAsciiMessage::decomposeRead()
{
    int offset = kResponseHeaderLen;
    for (const Element &element : prop_.Elements()) {
        switch (element.Databits()) {
            case ONE_BIT:
                for (int i = 0; i < element.Count(); ++i) {
                    result_.push_back(Ascii2Hex(response_.at(offset)));
                    offset += BYTE_SIZE;
                }
                break;

            case WORD_BITS:
                for (int i = 0; i < element.Count(); ++i) {
                    int unit_len = WORD_SIZE << 1;
                    ByteArray array(response_.begin() + offset,
                                    response_.begin() + offset + unit_len);

                    uint16_t value = Array2Value(array, WORD_SIZE, E_BE, true);
                    appendValue(result_, value, WORD_SIZE);

                    offset += unit_len;
                }
                break;

            case DWORD_BITS:
                for (int i = 0; i < element.Count(); ++i) {
                    int unit_len = DWORD_SIZE << 1;
                    ByteArray array(response_.begin() + offset,
                                    response_.begin() + offset + unit_len);

                    uint16_t value = Array2Value(array, DWORD_SIZE, E_BE, true);
                    appendValue(result_, value, DWORD_SIZE);

                    offset += unit_len;
                }
                break;

            default:
                error_ = E_INVALID_PARAMETER;
                return false;
                break;
        }
    }

    return true;
}

bool FatekAsciiMessage::decomposeWrite()
{
    int resp_len = response_.size();
    if (resp_len == (kResponseHeaderLen + kResponseTrailerLen)) {
        return true;
    }

    return false;
}

bool FatekAsciiMessage::decomposeCommon()
{
    // STX(1), STN(2), CMD(2), ERR(1), data, CSUM(2), ETX(1)
    int resp_len = response_.size();
    int data_len = resp_len - kResponseHeaderLen - kResponseTrailerLen;
    int unit_len = BYTE_SIZE << 1;

    for (int i = 0; i < data_len; i += unit_len) {
        ByteArray array(unit_len, 0);
        memcpy(array.data(),
               response_.data() + kResponseHeaderLen + i,
               unit_len);

        uint8_t value = Array2Value(array, BYTE_SIZE, E_BE, true);
        result_.push_back(value);
    }

    return true;
}

bool FatekAsciiMessage::decomposeLoopback()
{
    // STX(1), STN(2), CMD(2), data, CSUM(2), ETX(1)
    int resp_len = response_.size();
    int data_len = resp_len - kRequestHeaderLen - kRequestTrailerLen;

    for (int i = 0; i < data_len; ++i) {
        uint8_t data = response_.at(kRequestHeaderLen + i);
        result_.push_back(data);
    }

    return true;
}

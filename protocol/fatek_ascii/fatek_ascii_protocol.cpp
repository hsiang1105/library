#include "fatek_ascii_protocol.h"
#include "fatek_ascii.h"
#include "fatek_ascii_message.h"

using namespace Protocol;
using namespace FatekAscii;

namespace {
int getInterval(std::string &type)
{
    int interval = 0;
    int len = type.size();
    if (len == 0)
        return interval;

    switch (type[0]) {
        case 'W':
            interval = 16;
            break;
        case 'D':
            if (len > 2) {
                if (type[1] == 'W')
                    interval = 32;  // DWX, DWY, DWM, DWS
                else
                    interval = 1;  // DRC

            } else if (len > 1) {
                interval = 2;  // DD, DR, DF

            } else {
                interval = 1;
            }
            break;
        default:
            interval = 1;
            break;
    }

    return interval;
}
}  // namespace

FatekAsciiProtocol::FatekAsciiProtocol()
  : AbstractProtocol()
{
}

FatekAsciiProtocol::~FatekAsciiProtocol()
{
    for (auto it = message_map_.begin(); it != message_map_.end(); ++it) {
        delete it->second;
    }

    message_map_.clear();
}

Result FatekAsciiProtocol::GenerateRequest(const Property &prop)
{
    Result result;
    FatekAsciiMessage *message = new FatekAsciiMessage;
    if (message->SetProperty(prop)) {
        result.data = message->Request();
        message_map_[prop.Id()] = message;

    } else {
        result.status = message->Error();
        delete message;
    }

    return result;
}

Result FatekAsciiProtocol::HandleResponse(const Property &prop,
                                          const ByteArray &response)
{
    Result result;
    FatekAsciiMessage *message = message_map_.at(prop.Id());
    if (message) {
        if (message->SetResponse(response)) {
            result.data = message->Result();
        } else {
            result.status = message->Error();
        }

        message_map_.erase(prop.Id());
        delete message;

    } else {
        result.data.clear();
        result.status = E_PROPERTY_ERROR;
    }

    return result;
}

int FatekAsciiProtocol::ExpectedResponseLength(const Property &prop)
{
    int expected_resp_len = 0;
    FatekAsciiMessage *message = message_map_.at(prop.Id());
    if (message) {
        expected_resp_len = message->ExpectedResponseLength();
    }

    return expected_resp_len;
}

bool FatekAsciiProtocol::IsTerminated(const ByteArray &response)
{
    FatekAsciiMessage message;
    if (message.IsTerminated(response)) {
        return true;
    }

    return false;
}

PropertyList FatekAsciiProtocol::Fragment(const Property &prop)
{
    PropertyList prop_list;
    switch (prop.Function()) {
        case E_FUNC_BATCH_READ_VALUES:
        case E_FUNC_BATCH_READ_ENABLE_STATE:
            prop_list = fragmentBatchRead(prop);
            break;

        case E_FUNC_BATCH_WRITE_VALUES:
            prop_list = fragmentBatchWrite(prop);
            break;

        case E_FUNC_RAMDOM_READ_VALUES:
        case E_FUNC_MIX_READ_VALUES:
            prop_list = fragmentRandomRead(prop);
            break;

        case E_FUNC_RAMDOM_WRITE_VALUES:
        case E_FUNC_MIX_WRITE_VALUES:
            prop_list = fragmentRandomWrite(prop);
            break;

        default:
            prop_list.push_back(prop);
            break;
    }

    return prop_list;
}

PropertyList FatekAsciiProtocol::fragmentBatchRead(const Property &prop)
{
    return fragmentBatchWrite(prop);
}

PropertyList FatekAsciiProtocol::fragmentBatchWrite(const Property &prop)
{
    // batch property has only one element
    Element element = prop.Elements().front();
    std::string type = element.Type();
    int addr = element.Address();
    int databits = element.Databits();
    int count = element.Count();
    int data_bytes = element.DataBytes();
    const int data_size = data_bytes / count;
    const int interval = getInterval(type);

    // Bit -> 256 bytes
    // Word/Dword -> 128 bytes
    const int frament_len =
        (data_size == BYTE_SIZE) ? kFragmentLen : (kFragmentLen >> 1);

    // split element if data bytes of element exceed frament_len
    // ("M", 0, 16, 400, 400, false) ->
    // ("M", 0, 16, 256, 256, false), ("M", 256, 16, 144, 144, false)
    ElementArray split_elements;
    while (count > 0) {
        int split_count = 0;
        int split_data_bytes = 0;
        if (data_bytes > frament_len) {
            split_count = frament_len / data_size;
            split_data_bytes = split_count * data_size;
        } else {
            split_count = count;
            split_data_bytes = data_bytes;
        }

        split_elements.push_back(
            Element(type, addr, databits, split_count, split_data_bytes));

        addr += (split_count * interval);
        count -= split_count;
        data_bytes -= split_data_bytes;
    }

    // combine split element and split data into new property list
    PropertyList prop_list;
    ByteArray data = prop.Data();
    int data_offset = 0;
    for (const Element &split_element : split_elements) {
        // create split property
        Property split_prop(prop);
        split_prop.SetElements(ElementArray(1, split_element));

        if (!data.empty()) {
            ByteArray split_data;
            auto begin = data.begin() + data_offset;
            auto end = begin + split_element.DataBytes();
            split_data = ByteArray(begin, end);
            data_offset += split_element.DataBytes();
            split_prop.SetData(split_data);
        }

        prop_list.push_back(split_prop);
    }

    return prop_list;
}

PropertyList FatekAsciiProtocol::fragmentRandomRead(const Property &prop)
{
    return fragmentRandomWrite(prop);
}

PropertyList FatekAsciiProtocol::fragmentRandomWrite(const Property &prop)
{
    ElementArray split_elements;
    for (const Element &element : prop.Elements()) {
        std::string type = element.Type();
        int addr = element.Address();
        int databits = element.Databits();
        int count = element.Count();
        int data_bytes = element.DataBytes();
        const int data_size = data_bytes / count;
        const int interval = getInterval(type);

        // split element if element count is greater than 1
        // ("R", 0, 16, 2, 4, false) ->
        // ("R", 0, 16, 1, 2, false), ("R", 1, 16, 1, 2, false)
        while (count > 0) {
            int split_count = 1;
            int split_data_bytes = data_size;
            split_elements.push_back(
                Element(type, addr, databits, split_count, split_data_bytes));

            addr += (split_count * interval);
            count -= split_count;
            data_bytes -= split_data_bytes;
        }
    }

    // combine split element and split data into new property list
    PropertyList prop_list;
    ElementArray tmp_elements;
    ByteArray data = prop.Data();
    auto data_begin = data.begin();
    int tmp_data_bytes;
    for (const Element &split_element : split_elements) {
        if (tmp_elements.size() < kWriteRandomPoints) {
            tmp_elements.push_back(split_element);
            tmp_data_bytes += split_element.DataBytes();

        } else {
            // create split Property
            Property split_prop(prop);
            split_prop.SetElements(tmp_elements);

            if (!data.empty()) {
                ByteArray split_data;
                auto data_end = data_begin + tmp_data_bytes;
                split_data = ByteArray(data_begin, data_end);
                split_prop.SetData(split_data);
            }

            prop_list.push_back(split_prop);

            tmp_elements.clear();
            data_begin += tmp_data_bytes;
            tmp_data_bytes = 0;
        }
    }

    return prop_list;
}

PROTOCOL_API AbstractProtocol *createProtocol()
{
    return static_cast<AbstractProtocol *>(new FatekAsciiProtocol());
}

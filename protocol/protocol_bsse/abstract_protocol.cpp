#include "abstract_protocol.h"

namespace Protocol {

AbstractProtocol::~AbstractProtocol() {}

int AbstractProtocol::ExpectedResponseLength(const Protocol::Property &prop)
{
    _UNUSED(prop)
    return 0;
}

bool AbstractProtocol::IsTerminated(const ByteArray &response)
{
    _UNUSED(response)
    return false;
}

PropertyList AbstractProtocol::Fragment(const Protocol::Property &prop)
{
    PropertyList prop_list;
    prop_list.push_back(prop);

    return prop_list;
}

Protocol::Result AbstractProtocol::Simulate(const Protocol::Property &prop)
{
    _UNUSED(prop)
    return Protocol::Result();
}

int AbstractProtocol::Parameter(int type) const
{
    int param = -1;
    if (param_map_.find(type) != param_map_.end())
        param = param_map_.at(type);

    return param;
}

void AbstractProtocol::SetParameter(int param, int type)
{
    param_map_[type] = param;
}

}  // namespace Protocol

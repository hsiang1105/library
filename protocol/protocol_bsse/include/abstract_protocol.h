#ifndef ABSTRACT_PROTOCOL_H
#define ABSTRACT_PROTOCOL_H

#include <map>
#include "protocol_property.h"

#if defined(DLL_EXPORTS)
#define PROTOCOL_API __declspec(dllexport)
#else
#define PROTOCOL_API __declspec(dllimport)
#endif

namespace Protocol {

class AbstractProtocol
{
public:
    virtual ~AbstractProtocol();

    virtual Result GenerateRequest(const Protocol::Property &prop) = 0;
    virtual Result HandleResponse(const Property &prop,
                                  const ByteArray &response) = 0;

    virtual int ExpectedResponseLength(const Property &prop);
    virtual bool IsTerminated(const ByteArray &response);
    virtual PropertyList Fragment(const Property &prop);
    virtual Result Simulate(const Property &prop);

    int Parameter(int type) const;
    void SetParameter(int param, int type);

protected:
    std::map<int, int> param_map_;
};

}  // namespace Protocol

extern "C" PROTOCOL_API Protocol::AbstractProtocol *CreateProtocol();

#endif  // ABSTRACT_PROTOCOL_H

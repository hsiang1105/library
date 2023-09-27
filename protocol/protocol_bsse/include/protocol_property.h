#ifndef PROTOCOL_PROPERTY_H
#define PROTOCOL_PROPERTY_H

#include <list>
#include "protocol_element.h"

namespace Protocol {
class PropertyData;

class Property
{
public:
    Property();
    Property(const Property &source);
    ~Property();

    Property &operator=(const Property &other);

    int Id() const;
    std::string Name() const;
    uint32_t Function() const;
    ElementArray Elements() const;

    enum DataRole
    {
        E_QUERY_ROLE,
        E_REPLY_ROLE,
        E_USER_ROLE = 0x1000
    };

    bool HasData(int role = E_QUERY_ROLE) const;
    ByteArray Data(int role = E_QUERY_ROLE) const;
    int UserValue(int index = 0) const;

    void SetId(int id);
    void SetName(const std::string &name);
    void SetFunction(uint32_t function);
    void SetElements(const ElementArray &elements);
    void SetData(const ByteArray &data, int role = E_QUERY_ROLE);
    void SetUserValue(int value, int index = 0);

private:
    PropertyData *d_;
};

struct Result
{
    Result();
    ByteArray data;
    int status;
};

}  // namespace Protocol

#ifndef PropertyList
using PropertyList = std::list<Protocol::Property>;
#endif

#endif  // PROTOCOL_PROPERTY_H

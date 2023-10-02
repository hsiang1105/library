#ifndef PROTOCOL_ELEMENT_H
#define PROTOCOL_ELEMENT_H

#include <string>
#include "protocol.h"

namespace Protocol {
class ElementExtensionData;
class ElementData;

class ElementExtension
{
public:
    ElementExtension();
    ElementExtension(const ElementExtension &source);
    ~ElementExtension();

    ElementExtension &operator=(const ElementExtension &other);

    int TypeNumber(int index = 0) const;
    int SubAddress(int index = 0) const;
    int UserValue(int index = 0) const;
    ByteArray UserData(int index = 0) const;

    void SetTypeNumber(int number, int index = 0);
    void SetSubAddress(int address, int index = 0);
    void SetUserValue(int number, int index = 0);
    void SetUserData(const ByteArray &data, int index = 0);

private:
    ElementExtensionData *d_;
};

class Element
{
public:
    Element();
    Element(const std::string &type,
            int address,
            int databits,
            int count,
            int data_bytes,
            bool has_extension = false);

    Element(const Element &source);
    ~Element();

    Element &operator=(const Element &other);

    std::string Type() const;
    int Address() const;
    int Databits() const;
    int Count() const;
    int DataBytes() const;

    void SetType(const std::string &type);
    void SetAddress(int address);
    void SetDatabits(int databits);
    void SetCount(int count);
    void SetDataBytes(int data_bytes);

    bool HasExtension() const;
    void EnableExtension(bool enable);
    ElementExtension *Extension();
    const ElementExtension *Extension() const;

private:
    ElementData *d_;
};
}  // namespace Protocol

#ifndef ElementArray
using ElementArray = std::vector<Protocol::Element>;
#endif

#endif  // PROTOCOL_ELEMENT_H

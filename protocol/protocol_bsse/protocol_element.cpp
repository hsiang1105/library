#include "protocol_element.h"
#include <map>
#include "protocol.h"

namespace Protocol {
class ElementExtensionData
{
public:
    ElementExtensionData() {}

    std::map<int, int> type_no_map;
    std::map<int, int> sub_addr_map;
    std::map<int, int> user_value_map;
    std::map<int, ByteArray> user_data_map;
};

class ElementData
{
public:
    ElementData()
      : type("?elm")
      , addr(0)
      , databits(0)
      , count(0)
      , data_bytes(0)
      , extension(NULL)
    {
    }

    std::string type;
    int addr;
    int databits;
    int count;
    int data_bytes;
    ElementExtension *extension;
};

ElementExtension::ElementExtension()
  : d_(new ElementExtensionData)
{
}

ElementExtension::ElementExtension(const ElementExtension &source)
  : d_(new ElementExtensionData)
{
    *d_ = *source.d_;
}

ElementExtension::~ElementExtension()
{
    if (d_) {
        delete d_;
        d_ = NULL;
    }
}

ElementExtension &ElementExtension::operator=(const ElementExtension &other)
{
    if (this != &other) {
        *d_ = *other.d_;
    }

    return *this;
}

int ElementExtension::TypeNumber(int index) const
{
    int number = -1;
    if (d_->type_no_map.find(index) != d_->type_no_map.end())
        number = d_->type_no_map.at(index);

    return number;
}

int ElementExtension::SubAddress(int index) const
{
    int addr = -1;
    if (d_->sub_addr_map.find(index) != d_->sub_addr_map.end())
        addr = d_->sub_addr_map.at(index);

    return addr;
}

int ElementExtension::UserValue(int index) const
{
    int number = -1;
    if (d_->user_value_map.find(index) != d_->user_value_map.end())
        number = d_->user_value_map.at(index);

    return number;
}

ByteArray ElementExtension::UserData(int index) const
{
    ByteArray data;
    if (d_->user_data_map.find(index) != d_->user_data_map.end())
        data = d_->user_data_map.at(index);

    return data;
}

void ElementExtension::SetTypeNumber(int number, int index)
{
    d_->type_no_map[index] = number;
}

void ElementExtension::SetSubAddress(int address, int index)
{
    d_->sub_addr_map[index] = address;
}

void ElementExtension::SetUserValue(int number, int index)
{
    d_->user_value_map[index] = number;
}

void ElementExtension::SetUserData(const ByteArray &data, int index)
{
    d_->user_data_map[index] = data;
}

Element::Element()
  : d_(new ElementData)
{
}

Element::Element(const std::string &type,
                 int address,
                 int databits,
                 int count,
                 int data_bytes,
                 bool has_extension)
  : d_(new ElementData)
{
    d_->type = type;
    d_->addr = address;
    d_->databits = databits;
    d_->count = count;
    d_->data_bytes = data_bytes;
    if (has_extension) {
        d_->extension = new ElementExtension();
    }
}

Element::Element(const Element &source)
  : d_(new ElementData)
{
    d_->type = source.d_->type;
    d_->addr = source.d_->addr;
    d_->databits = source.d_->databits;
    d_->count = source.d_->count;
    d_->data_bytes = source.d_->data_bytes;
    if (source.d_->extension) {
        d_->extension = new ElementExtension(*source.d_->extension);
    }
}

Element::~Element()
{
    if (d_) {
        delete d_;
        d_ = NULL;
    }
}

Element &Element::operator=(const Element &other)
{
    if (this != &other) {
        d_->type = other.d_->type;
        d_->addr = other.d_->addr;
        d_->databits = other.d_->databits;
        d_->count = other.d_->count;
        d_->data_bytes = other.d_->data_bytes;

        if (d_->extension) {
            delete d_->extension;
        }

        d_->extension = new ElementExtension(*other.d_->extension);
    }

    return *this;
}

std::string Element::Type() const
{
    return d_->type;
}

int Element::Address() const
{
    return d_->addr;
}

int Element::Databits() const
{
    return d_->databits;
}

int Element::Count() const
{
    return d_->count;
}

int Element::DataBytes() const
{
    return d_->data_bytes;
}

void Element::SetType(const std::string &type)
{
    d_->type = type;
}

void Element::SetAddress(int address)
{
    d_->addr = address;
}

void Element::SetDatabits(int databits)
{
    d_->databits = databits;
}

void Element::SetCount(int count)
{
    d_->count = count;
}

void Element::SetDataBytes(int data_bytes)
{
    d_->data_bytes = data_bytes;
}

bool Element::HasExtension() const
{
    if (d_->extension) {
        return true;
    }

    return false;
}

void Element::EnableExtension(bool enable)
{
    if (enable) {
        if (d_->extension == NULL) {
            d_->extension = new ElementExtension;
        }

    } else {
        if (d_->extension) {
            delete d_->extension;
            d_->extension = NULL;
        }
    }
}

ElementExtension *Element::Extension()
{
    return d_->extension;
}

const ElementExtension *Element::Extension() const
{
    return d_->extension;
}

}  // namespace Protocol

#include "protocol_property.h"
#include <map>

namespace Protocol {
class PropertyData
{
public:
    PropertyData()
      : id(-1)
      , name("?prop")
      , function(0)
      , station(0)

    {
    }

    int id;
    std::string name;
    uint32_t function;
    int station;
    ElementArray elements;
    std::map<int, ByteArray> data_map;
    std::map<int, int> user_value_map;
};

Property::Property()
  : d_(new PropertyData)
{
}

Property::Property(const Property &source)
  : d_(new PropertyData)
{
    d_->id = source.d_->id;
    d_->name = source.d_->name;
    d_->function = source.d_->function;
    d_->station = source.d_->station;
    d_->elements = source.d_->elements;
    d_->data_map = source.d_->data_map;
    d_->user_value_map = source.d_->user_value_map;
}

Property::~Property()
{
    if (d_) {
        delete d_;
        d_ = NULL;
    }
}

Property &Property::operator=(const Property &other)
{
    if (this != &other) {
        d_->id = other.d_->id;
        d_->name = other.d_->name;
        d_->function = other.d_->function;
        d_->station = other.d_->station;
        d_->elements = other.d_->elements;
        d_->data_map = other.d_->data_map;
        d_->user_value_map = other.d_->user_value_map;
    }

    return *this;
}

int Property::Id() const
{
    return d_->id;
}

int Property::Station() const
{
    return d_->station;
}

std::string Property::Name() const
{
    return d_->name;
}

uint32_t Property::Function() const
{
    return d_->function;
}

ElementArray Property::Elements() const
{
    return d_->elements;
}

bool Property::HasData(int role) const
{
    if (d_->data_map.find(role) != d_->data_map.end()) {
        return true;
    }

    return false;
}

ByteArray Property::Data(int role) const
{
    ByteArray data;
    if (d_->data_map.find(role) != d_->data_map.end())
        data = d_->data_map.at(role);

    return data;
}

int Property::UserValue(int index) const
{
    int value = -1;
    if (d_->user_value_map.find(index) != d_->user_value_map.end())
        value = d_->user_value_map.at(index);

    return value;
}

void Property::SetId(int id)
{
    d_->id = id;
}

void Property::SetStation(int station)
{
    d_->station = station;
}

void Property::SetName(const std::string &name)
{
    d_->name = name;
}

void Property::SetFunction(uint32_t function)
{
    d_->function = function;
}

void Property::SetElements(const ElementArray &elements)
{
    d_->elements = elements;
}

void Property::SetData(const ByteArray &data, int role)
{
    d_->data_map[role] = data;
}

void Property::SetUserValue(int value, int index)
{
    d_->user_value_map[index] = value;
}

Result::Result()
{
    this->status = E_NO_ERROR;
}

}  // namespace Protocol

#include "protocol.h"

namespace {
uint8_t g_modbus_crc_hi[] =
    {0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
     0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
     0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
     0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
     0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
     0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
     0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
     0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
     0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
     0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
     0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
     0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
     0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
     0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
     0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
     0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
     0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
     0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
     0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
     0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
     0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
     0x00, 0xC1, 0x81, 0x40};

uint8_t g_modbus_crc_lo[] =
    {0x00, 0xc0, 0xc1, 0x01, 0xc3, 0x03, 0x02, 0xc2, 0xc6, 0x06, 0x07, 0xc7,
     0x05, 0xc5, 0xc4, 0x04, 0xcc, 0x0c, 0x0d, 0xcd, 0x0f, 0xcf, 0xce, 0x0e,
     0x0a, 0xca, 0xcb, 0x0b, 0xc9, 0x09, 0x08, 0xc8, 0xd8, 0x18, 0x19, 0xd9,
     0x1b, 0xdb, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
     0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
     0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
     0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
     0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
     0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
     0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
     0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
     0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
     0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
     0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
     0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
     0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
     0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
     0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
     0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
     0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
     0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
     0x41, 0x81, 0x80, 0x40};
}  // namespace

uint8_t Protocol::Ascii2Hex(uint8_t ascii)
{
    uint8_t hex = 0;
    if (ascii >= 0x30 && ascii <= 0x39) {
        hex = ascii - 48;
    } else if (ascii >= 0x41 && ascii <= 0x46) {
        hex = ascii - 55;
    }

    return hex;
}

uint8_t Protocol::Hex2Ascii(uint8_t hex)
{
    char ascii = '!';
    if (hex <= 9) {
        ascii = hex + 48;
    } else if (hex <= 15) {
        ascii = hex + 55;
    }

    return ascii;
}

ByteArray Protocol::Value2Array(uint32_t value,
                                int size,
                                ByteOrder endian,
                                bool ascii_flag)
{
    int array_len = (ascii_flag) ? (size << 1) : size;
    ByteArray array(array_len, 0);
    int offset = 0;
    for (int i = 0; i < size; ++i) {
        uint8_t tmp = 0;
        int shift = 0;
        if (endian == E_BE)
            shift = (size - i - 1) << 3;
        else
            shift = i << 3;

        tmp = (value >> shift) & 0xFF;

        if (ascii_flag) {
            array[offset++] = Hex2Ascii((tmp >> 4) & 0x0F);
            array[offset++] = Hex2Ascii(tmp & 0xF);
        } else {
            array[offset++] = tmp;
        }
    }

    return array;
}

uint32_t Protocol::Array2Value(const ByteArray &array,
                               int size,
                               ByteOrder endian,
                               bool ascii_flag)
{
    uint32_t value = 0;
    int array_len = (ascii_flag) ? (size << 1) : size;
    if (array_len > (int)array.size())
        return value;

    int i = 0;
    while (i < array_len) {
        for (int j = 0; j < size; ++j) {
            uint8_t tmp = 0;
            int shift = 0;
            if (ascii_flag) {
                tmp = (Ascii2Hex(array[i++]) << 4) & 0xF0;
                tmp |= Ascii2Hex(array[i++]);
            } else {
                tmp = array[i++];
            }

            if (endian == E_BE)
                shift = (size - j - 1) << 3;
            else
                shift = j << 3;

            value |= (tmp << shift);
        }
    }

    return value;
}

void Protocol::AppendValue(ByteArray &result,
                           uint32_t value,
                           int size,
                           Protocol::ByteOrder endian,
                           bool ascii_flag)
{
    ByteArray array = Value2Array(value, size, endian, ascii_flag);
    result.insert(result.end(), array.begin(), array.end());
}

uint16_t Protocol::Checksum(const char *start, int offset, int len)
{
    const char *data = start + offset;
    uint32_t csum = 0;
    for (int i = 0; i < len; i++)
        csum += data[i];

    return (csum & 0xFFFF);
}

uint16_t Protocol::CRC16(const char *start, int offset, int len)
{
    const char *data = start + offset;
    const uint8_t *byte = reinterpret_cast<const uint8_t *>(data);
    uint8_t crc_lo = 0xFF;
    uint8_t crc_hi = 0xFF;

    while (len--) {
        uint16_t idx = crc_hi ^ *byte++;
        crc_hi = crc_lo ^ g_modbus_crc_hi[idx];
        crc_lo = g_modbus_crc_lo[idx];
    }

    uint16_t crc = (uint16_t)((crc_hi << BYTE_BITS) | crc_lo);

    return crc;
}

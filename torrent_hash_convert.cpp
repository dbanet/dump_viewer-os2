#include "torrent_hash_convert.h"
#include "arpmanetdc/util.h"

QString hex_to_base32(const QString& hex) {
    QByteArray data = QByteArray::fromHex(hex.toUtf8());
    base32Encode(data);
    return data;
}

QString base32_to_hex(const QString& base32) {
    QByteArray data(base32.toUtf8());
    base32Decode(data);
    return data.toHex().toUpper();
}

QString torrent_hash_convert(const QString& input, bool output_base32) {
    bool input_base32 = input.length() == 32;
    if (input_base32 == output_base32) {
        return input;
    } else if (input_base32) {
        return base32_to_hex(input);
    } else {
        return hex_to_base32(input);
    }
}

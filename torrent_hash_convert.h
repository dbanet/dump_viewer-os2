#ifndef TORRENT_HASH_CONVERT_H
#define TORRENT_HASH_CONVERT_H

#include <QtCore>

QString hex_to_base32(const QString& hex);

QString base32_to_hex(const QString& base32);

// guess input by length
QString torrent_hash_convert(const QString& input, bool output_base32);

#endif // TORRENT_HASH_CONVERT_H

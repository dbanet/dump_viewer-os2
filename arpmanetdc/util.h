#ifndef UTIL_H
#define UTIL_H

#if QT_VERSION < 0x040800
#define Q_LIKELY(expr) ( expr )
#endif
#if QT_VERSION < 0x040800
#define Q_UNLIKELY(expr) ( expr )
#endif

#include <QObject>
#include <QByteArray>
#include <QChar>
#include <QRegExp>
#include <QString>
#include <QDataStream>
#include <QTableView>
#include "base32.h"

typedef unsigned char byte;
static const char *base32Alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"; //Used for Base32 conversions - RFC4648 standard

//Encoded an UNSIGNED CHAR *input into a QByteArray
QByteArray base32Encode(byte *input, int inputLength);
//Decodes a QByteArray into an UNSIGNED CHAR *output
//You need to call Base32DecodedLength to get outputLength before calling this function
bool base32Decode(QByteArray input, byte *output, int &outputLength);

//More intuitive base32 methods - returns true if conversion succeeded
bool base32Encode(QByteArray &data);
bool base32Decode(QByteArray &data);

//Function to convert quint64 containing bytes into a human readable format
QString bytesToSize(quint64 bytes);
//Function to convert bytes/second quint64 to human readable format
QString bytesToRate(quint64 rate);
//Function to convert human readable size into bytes
quint64 sizeToBytes(QString size);
//Function to convert human readable rate into bytes
quint64 rateToBytes(QString rate);
//Function to adjust a tableviews rows to fit contents (much faster fixed size version)
void resizeRowsToContents(QTableView *tableView);

QByteArray toQByteArray(quint16 n);
QByteArray toQByteArray(quint32 n);
QByteArray toQByteArray(quint64 n);

//Functions to convert basic types to QByteArray
QByteArray quint16ToByteArray(quint16 num);
QByteArray quint8ToByteArray(quint8 num);
QByteArray qint16ToByteArray(qint16 num);
QByteArray quint64ToByteArray(quint64 num);
QByteArray quint32ToByteArray(quint32 num);
QByteArray stringToByteArray(QString str);
QByteArray sizeOfByteArray(QByteArray *data);

//Functions to extract a basic type from a QByteArray
QString getStringFromByteArray(QByteArray *data);
quint16 getQuint16FromByteArray(QByteArray *data);
quint8 getQuint8FromByteArray(QByteArray *data);
qint16 getQint16FromByteArray(QByteArray *data);
quint32 getQuint32FromByteArray(QByteArray *data);
quint64 getQuint64FromByteArray(QByteArray *data);



#endif // UTIL_H

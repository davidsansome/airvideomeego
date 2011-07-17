#include "wireformat.h"

#include <QIODevice>
#include <QtDebug>

#include <arpa/inet.h>


AVDict::AVDict(const QString& name)
  : name_(name)
{
}

QDebug operator <<(QDebug s, const AVDict& d) {
  s.nospace() << "AVDict(" << d.name() << ", "
              << QMap<QString, QVariant>(d)
              << ")";
  return s.maybeSpace();
}


AVStream::AVStream(QIODevice* device)
  : device_(device)
{
}

void AVStream::WriteRawInt(int value) {
  const quint32 big_endian_value = htonl(value);
  device_->write(reinterpret_cast<const char*>(&big_endian_value), 4);
}

void AVStream::Write(int value, int) {
  device_->write("i", 1);
  WriteRawInt(value);
}

void AVStream::Write(double value, int) {
  device_->write("f", 1);

  QByteArray data(8, '\0');
  memcpy(data.data(), &value, 8);
  NtohDouble(&data);
  device_->write(data);
}

void AVStream::Write(const QString& value, int counter) {
  if (value.isNull()) {
    device_->write("n", 1);
  } else {
    device_->write("s", 1);
    WriteRawInt(counter);
    WriteRawInt(value.length());
    device_->write(value.toUtf8());
  }
}

void AVStream::Write(const AVDict& value, int counter) {
  int version = 1;
  if (value.name() == "air.video.ConversionRequest" ||
      value.name() == "air.video.BrowseRequest") {
    version = 240;
  }

  device_->write("o", 1);
  WriteRawInt(counter);
  WriteRawInt(value.name().length());
  device_->write(value.name().toUtf8());
  WriteRawInt(version);
  WriteRawInt(value.count());

  foreach (const QString& key, value.keys()) {
    WriteRawInt(key.length());
    device_->write(key.toUtf8());

    Write(value[key], counter + 1);
  }
}

void AVStream::Write(const QVariantList& value, int counter) {
  device_->write("a", 1);
  WriteRawInt(counter);
  WriteRawInt(value.count());

  foreach (const QVariant& item, value) {
    Write(item);
  }
}

void AVStream::Write(const AVBitrateList& value, int counter) {
  device_->write("e", 1);
  WriteRawInt(counter);
  WriteRawInt(value.count());

  foreach (const QVariant& item, value) {
    Write(item);
  }
}

void AVStream::Write(const QVariant& variant, int counter) {
  if (variant.type() == QVariant::String) {
    Write(variant.toString(), counter);
  } else if (variant.type() == QVariant::List) {
    Write(variant.toList(), counter);
  } else if (variant.canConvert<AVDict>()) {
    Write(variant.value<AVDict>(), counter);
  } else if (variant.type() == QVariant::Double ||
             variant.type() == QVariant::Type(QMetaType::Float)) {
    Write(float(variant.toDouble()), counter);
  } else if (variant.canConvert(QVariant::Int)) {
    Write(variant.toInt(), counter);
  } else {
    qWarning() << "Unknown type in QVariant:" << variant.type() << variant.typeName();
  }
}

int AVStream::ReadRawInt() {
  quint32 big_endian_value = 0;
  device_->read(reinterpret_cast<char*>(&big_endian_value), 4);

  quint32 value = ntohl(big_endian_value);
  return *reinterpret_cast<int*>(&value);
}

int AVStream::ReadInt() {
  return ReadRawInt();
}

void AVStream::NtohDouble(QByteArray* value) {
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
  char* data = value->data();

  std::swap(data[0], data[7]);
  std::swap(data[1], data[6]);
  std::swap(data[2], data[5]);
  std::swap(data[3], data[4]);
#endif
}

double AVStream::ReadDouble() {
  QByteArray value = device_->read(8);
  NtohDouble(&value);

  return *reinterpret_cast<const double*>(value.constData());
}

QString AVStream::ReadString() {
  return QString::fromUtf8(ReadData());
}

AVDict AVStream::ReadDict() {
  const int counter = ReadRawInt(); Q_UNUSED(counter);
  const int name_length = ReadRawInt();

  AVDict ret(QString::fromUtf8(device_->read(name_length)));

  const int version = ReadRawInt(); Q_UNUSED(version);
  const int count = ReadRawInt();

  for (int i=0 ; i<count ; ++i) {
    const int key_length = ReadRawInt();
    QString key(QString::fromUtf8(device_->read(key_length)));

    ret[key] = Read();
  }
  return ret;
}

QVariantList AVStream::ReadList() {
  const int counter = ReadRawInt(); Q_UNUSED(counter);
  const int count = ReadRawInt();

  QVariantList ret;

  for (int i=0 ; i<count ; ++i) {
    ret << Read();
  }
  return ret;
}

QByteArray AVStream::ReadData() {
  const int counter = ReadRawInt(); Q_UNUSED(counter);
  const int length = ReadRawInt();
  return device_->read(length);
}

QVariant AVStream::Read() {
  char type = '\0';
  if (device_->read(&type, 1) != 1) {
    return QVariant();
  }

  switch (type) {
    case 'i': return ReadInt();
    case 'f': return ReadDouble();
    case 's': return ReadString();
    case 'n': return QVariant();
    case 'o': return QVariant::fromValue(ReadDict());
    case 'a':
    case 'e':
    case 'd': return ReadList();
    case 'x': return ReadData();
    default:
      qWarning() << "Unknown type in stream:" << int(type) << type;
      return QVariant();
  }
}

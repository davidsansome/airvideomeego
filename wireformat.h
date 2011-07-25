#ifndef WIREFORMAT_H
#define WIREFORMAT_H

#include <QDataStream>
#include <QDebug>
#include <QMap>
#include <QVariant>

class QIODevice;


class AVDict : public QMap<QString, QVariant> {
public:
  AVDict(const QString& name = QString());

  const QString& name() const { return name_; }

private:
  QString name_;
};

QDebug operator <<(QDebug s, const AVDict& d);
Q_DECLARE_METATYPE(AVDict)


class AVBitrateList : public QList<int> {
};

Q_DECLARE_METATYPE(AVBitrateList)


class AVStream {
public:
  AVStream(QIODevice* device);

  void Write(qint32 value, int counter = 0);
  void Write(double value, int counter = 0);
  void Write(const QString& value, int counter = 0);
  void Write(const AVDict& value, int counter = 0);
  void Write(const QVariantList& value, int counter = 0);
  void Write(const AVBitrateList& value, int counter = 0);
  void Write(const QVariant& value, int counter = 0);

  qint32 ReadInt32();
  quint32 ReadUInt32();
  qint64 ReadInt64();
  double ReadDouble();
  QString ReadString();
  AVDict ReadDict();
  QVariantList ReadList();
  QByteArray ReadData();

  QVariant Read();

private:
  void WriteRawInt(int value);
  int ReadRawInt();
  static void Ntoh64(QByteArray* value);
  static void Ntoh32(QByteArray* value);

private:
  QIODevice* device_;
};

#endif // WIREFORMAT_H

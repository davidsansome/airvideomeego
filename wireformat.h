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

  void Write(int value, int counter = 0);
  void Write(double value, int counter = 0);
  void Write(const QString& value, int counter = 0);
  void Write(const AVDict& value, int counter = 0);
  void Write(const QVariantList& value, int counter = 0);
  void Write(const AVBitrateList& value, int counter = 0);
  void Write(const QVariant& value, int counter = 0);

  int ReadInt();
  double ReadDouble();
  QString ReadString();
  AVDict ReadDict();
  QVariantList ReadList();
  QByteArray ReadData();

  QVariant Read();

private:
  void WriteRawInt(int value);
  int ReadRawInt();
  static void NtohDouble(QByteArray* value);

private:
  QIODevice* device_;
};

#endif // WIREFORMAT_H

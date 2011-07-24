#ifndef AVCLIENT_H
#define AVCLIENT_H

#include "videomodel.h"

#include <QMetaType>
#include <QString>
#include <QUrl>

class AVDict;

class QNetworkAccessManager;
class QNetworkReply;


class AVClient {
public:
  AVClient(const QString& hostname, int port = kDefaultPort);
  ~AVClient();

  static const int kDefaultPort = 45631;
  static const char* kClientIdentifier;

  struct Object {
    Object() : type_(VideoModel::Type_Unknown) {}

    VideoModel::Type type_;
    QString name_;
    QString item_id_;
    QString parent_id_;
  };

  struct MediaInfo {
    MediaInfo() : valid_(false), filesize_(0), duration_(0.0) {}

    bool valid_;
    qint64 filesize_;  // bytes
    double duration_;  // seconds
    QByteArray thumbnail_;
  };


  QNetworkReply* Browse(const QString& parent_id);
  QList<Object> ParseBrowseReply(QNetworkReply* reply);

  QNetworkReply* GetMediaInfo(const QString& item_id);
  AVClient::MediaInfo ParseGetMediaInfoReply(QNetworkReply* reply);

private:
  Q_DISABLE_COPY(AVClient)
  QNetworkReply* Request(const char* service, const char* method, const QVariant& params);

private:
  QNetworkAccessManager* network_;
  QUrl url_;
};

#endif // AVCLIENT_H

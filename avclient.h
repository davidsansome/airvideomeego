#ifndef AVCLIENT_H
#define AVCLIENT_H

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
    enum Type {
      Type_Unknown,
      Type_Folder,
      Type_Video,
    };

    Type type_;
    QString name_;
    QString item_id_;
    QString parent_id_;
  };


  QNetworkReply* Browse(const QString& parent_id);
  QList<Object> ParseBrowseReply(QNetworkReply* reply);

private:
  Q_DISABLE_COPY(AVClient)
  QNetworkReply* Request(const char* service, const char* method, const AVDict& params);

private:
  QNetworkAccessManager* network_;
  QUrl url_;
};

#endif // AVCLIENT_H

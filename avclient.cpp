#include "avclient.h"
#include "wireformat.h"

#include <QBuffer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtDebug>

const char* AVClient::kClientIdentifier = "89eae483355719f119d698e8d11e8b356525ecfb";


AVClient::AVClient(const QString& hostname, int port)
  : network_(new QNetworkAccessManager)
{
  url_.setScheme("http");
  url_.setHost(hostname);
  url_.setPort(port);
  url_.setPath("service");
}

AVClient::~AVClient() {
  delete network_;
}

QNetworkReply* AVClient::Browse(const QString& parent_id) {
  AVDict params("air.video.BrowseRequest");
  params["folderId"] = parent_id;
  params["preloadDetails"] = false;

  qDebug() << "Browsing" << parent_id;

  return Request("browseService", "getItems", QVariant::fromValue(params));
}

QNetworkReply* AVClient::Request(const char* service, const char* method,
                                 const QVariant& param) {
  QNetworkRequest req(url_);
  req.setRawHeader("User-Agent", "AirVideo/2.4.0 CFNetwork/459 Darwin/10.0.0d3");
  req.setRawHeader("Accept-Language", "en-us");

  AVDict data("air.connect.Request");
  data["requestURL"] = url_.toString();
  data["clientVersion"] = 240;
  data["serviceName"] = service;
  data["methodName"] = method;
  data["clientIdentifier"] = kClientIdentifier;
  data["parameters"] = QVariantList() << param;

  QBuffer buffer;
  buffer.open(QIODevice::WriteOnly);

  AVStream stream(&buffer);
  stream.Write(data);
  buffer.close();

  return network_->post(req, buffer.buffer());
}

QList<AVClient::Object> AVClient::ParseBrowseReply(QNetworkReply* reply) {
  QList<Object> ret;

  AVStream stream(reply);
  QVariant data(stream.Read());
  if (!data.canConvert<AVDict>()) {
    qWarning() << "Browse reply was not a dict";
    return ret;
  }

  AVDict dict = data.value<AVDict>();
  if (dict.name() != "air.connect.Response" || !dict.contains("result")) {
    qWarning() << "Unexpected dict type" << dict.name();
    return ret;
  }

  dict = dict["result"].value<AVDict>();
  if (dict.name() != "air.video.FolderContent" || !dict.contains("items")) {
    qWarning() << "Unexpected result dict type" << dict.name();
    return ret;
  }

  QVariantList items = dict["items"].toList();
  foreach (const QVariant& item, items) {
    if (!item.canConvert<AVDict>()) {
      qWarning() << "Browse item was not a dict";
      continue;
    }

    AVDict data(item.value<AVDict>());

    Object object;
    if (data.name() == "air.video.DiskRootFolder" ||
        data.name() == "air.video.Folder") {
      object.type_ = VideoModel::Type_Folder;
    } else if (data.name() == "air.video.VideoItem") {
      object.type_ = VideoModel::Type_Video;
    } else {
      object.type_ = VideoModel::Type_Unknown;
    }
    object.name_ = data["name"].toString();
    object.item_id_ = data["itemId"].toString();
    ret << object;

    qWarning() << "Browse reply contained" << object.name_ << object.item_id_;
  }

  return ret;
}

QNetworkReply* AVClient::GetMediaInfo(const QString& item_id) {
  return Request("browseService", "getItemsWithDetail", QVariantList() << item_id);
}

AVClient::MediaInfo AVClient::ParseGetMediaInfoReply(QNetworkReply* reply) {
  MediaInfo ret;

  AVStream stream(reply);
  QVariant data(stream.Read());
  if (!data.canConvert<AVDict>()) {
    qWarning() << "GetDetails reply was not a dict";
    return ret;
  }

  AVDict dict = data.value<AVDict>();
  if (dict.name() != "air.connect.Response" || !dict.contains("result")) {
    qWarning() << "Unexpected dict type" << dict.name();
    return ret;
  }

  QVariantList results = dict["result"].toList();
  if (results.count() != 1) {
    qWarning() << "GetDetails contained" << results.count() << "results";
    return ret;
  }

  dict = results[0].value<AVDict>();
  if (dict.name() != "air.video.VideoItem" || !dict.contains("detail")) {
    qWarning() << "GetDetails result of bad type" << dict.name();
    return ret;
  }

  dict = dict["detail"].value<AVDict>();
  if (dict.name() != "air.video.MediaInfo") {
    qWarning() << "GetDetails detail of bad type" << dict.name();
    return ret;
  }

  ret.valid_ = true;
  ret.filesize_ = dict["fileSize"].toLongLong();
  ret.duration_ = dict["duration"].toDouble();
  ret.thumbnail_ = dict["videoThumbnail"].toByteArray();
  qDebug() << ret.filesize_ << ret.duration_ << ret.thumbnail_.size();

  return ret;
}

#include "avclient.h"
#include "videomodel.h"

#include <QNetworkReply>
#include <QtDebug>

VideoModel::VideoModel(QObject* parent)
  : QStandardItemModel(parent),
    client_(new AVClient("192.168.2.14"))
{
  QHash<int, QByteArray> role_names = roleNames();
  role_names[Role_Name]             = "name";
  role_names[Role_ID]               = "id";
  role_names[Role_Type]             = "type";
  role_names[Role_IsContentsLoaded] = "is_contents_loaded";
  role_names[Role_IsVideoValid]     = "is_video_valid";
  role_names[Role_FileSize]         = "file_size";
  role_names[Role_Duration]         = "duration";

  setRoleNames(role_names);

  Reset();
}

VideoModel::~VideoModel() {
}

QString VideoModel::model_id() const {
  return QString::number(qlonglong(this), 16);
}

QString VideoModel::thumbnail_provider_name() const {
  return "thumbnails_" + model_id();
}

void VideoModel::Reset() {
  QList<QNetworkReply*> replies;
  replies << browse_requests_.keys();

  clear();
  items_by_id_.clear();
  browse_requests_.clear();

  qDeleteAll(replies);

  items_by_id_[QString()] = invisibleRootItem();
  Browse(QString());
}

void VideoModel::Browse(const QString& parent_id) {
  if (browse_requests_.values().contains(parent_id)) {
    return;
  }

  QNetworkReply* reply = client_->Browse(parent_id);
  connect(reply, SIGNAL(finished()), SLOT(BrowseRequestFinished()));

  browse_requests_[reply] = parent_id;
}

void VideoModel::BrowseRequestFinished() {
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
  reply->deleteLater();
  if (!browse_requests_.contains(reply)) {
    return;
  }

  QString parent_id = browse_requests_.take(reply);
  QStandardItem* parent_item = items_by_id_[parent_id];
  parent_item->setData(true, Role_IsContentsLoaded);

  QList<AVClient::Object> children = client_->ParseBrowseReply(reply);

  foreach (const AVClient::Object& child, children) {
    // Create an item for this child
    QStandardItem* item = new QStandardItem;
    item->setData(child.name_, Role_Name);
    item->setData(child.item_id_, Role_ID);
    item->setData(child.type_, Role_Type);
    item->setData(false, Role_IsContentsLoaded);

    item->setText(child.name_);

    items_by_id_[child.item_id_] = item;
    parent_item->appendRow(item);

    // Get info about the child if it's a video
    if (child.type_ == Type_Video) {
      QNetworkReply* details_reply = client_->GetMediaInfo(child.item_id_);
      connect(details_reply, SIGNAL(finished()), SLOT(GetMediaInfoRequestFinished()));

      media_info_requests_[details_reply] = child.item_id_;
    }
  }
}

void VideoModel::GetMediaInfoRequestFinished() {
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
  reply->deleteLater();
  if (!media_info_requests_.contains(reply)) {
    return;
  }

  QString item_id = media_info_requests_.take(reply);
  QStandardItem* item = items_by_id_[item_id];

  AVClient::MediaInfo info = client_->ParseGetMediaInfoReply(reply);

  qDebug() << "Setting data for" << item_id;
  if (info.valid_) {
    item->setData(info.duration_, Role_Duration);
    item->setData(info.filesize_, Role_FileSize);
    item->setData(info.thumbnail_, Role_ThumbnailData);
  }
  item->setData(info.valid_, Role_IsVideoValid);
  item->setData(true, Role_IsContentsLoaded);

  qDebug() << "Finished setting data for" << item_id;
}

bool VideoModel::hasChildren(const QModelIndex& parent) const {
  if (parent.data(Role_Type).toInt() == Type_Folder &&
      !parent.data(Role_IsContentsLoaded).toBool()) {
    return true;
  }

  return QStandardItemModel::hasChildren(parent);
}

bool VideoModel::canFetchMore(const QModelIndex& parent) const {
  return parent.data(Role_Type).toInt() == Type_Folder &&
         !parent.data(Role_IsContentsLoaded).toBool() &&
         !browse_requests_.values().contains(parent.data(Role_ID).toString());
}

void VideoModel::fetchMore(const QModelIndex& parent) {
  if (canFetchMore(parent)) {
    Browse(parent.data(Role_ID).toString());
  }
}

QByteArray VideoModel::ThumbnailData(const QString& item_id) const {
  qDebug() << "Getting data for" << item_id;
  if (items_by_id_.contains(item_id)) {
    return items_by_id_[item_id]->data(Role_ThumbnailData).toByteArray();
  }
  return QByteArray();
}

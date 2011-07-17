#include "avclient.h"
#include "videomodel.h"

#include <QNetworkReply>
#include <QtDebug>

VideoModel::VideoModel(QObject* parent)
  : QStandardItemModel(parent),
    client_(new AVClient("192.168.2.14"))
{
  QHash<int, QByteArray> role_names = roleNames();
  role_names[Role_Name] = "foobar";
  role_names[Role_ID] = "id";
  role_names[Role_Type] = "type";
  setRoleNames(role_names);
  qDebug() << role_names;

  Reset();
}

VideoModel::~VideoModel() {
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

  qDebug() << "Appending" << children.count() << "items to" << parent_item->text();

  foreach (const AVClient::Object& child, children) {
    QStandardItem* item = new QStandardItem;
    item->setData(child.name_, Role_Name);
    item->setData(child.item_id_, Role_ID);
    item->setData(child.type_, Role_Type);
    item->setData(false, Role_IsContentsLoaded);

    item->setText(child.name_);

    items_by_id_[child.item_id_] = item;
    parent_item->appendRow(item);
  }
}

bool VideoModel::hasChildren(const QModelIndex& parent) const {
  if (parent.data(Role_Type).toInt() == AVClient::Object::Type_Folder &&
      !parent.data(Role_IsContentsLoaded).toBool()) {
    return true;
  }

  return QStandardItemModel::hasChildren(parent);
}

bool VideoModel::canFetchMore(const QModelIndex& parent) const {
  return parent.data(Role_Type).toInt() == AVClient::Object::Type_Folder &&
         !parent.data(Role_IsContentsLoaded).toBool() &&
         !browse_requests_.values().contains(parent.data(Role_ID).toString());
}

void VideoModel::fetchMore(const QModelIndex& parent) {
  if (canFetchMore(parent)) {
    Browse(parent.data(Role_ID).toString());
  }
}

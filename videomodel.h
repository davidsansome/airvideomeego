#ifndef VIDEOMODEL_H
#define VIDEOMODEL_H

#include <QScopedPointer>
#include <QStandardItemModel>

class AVClient;

class QNetworkReply;


class VideoModel : public QStandardItemModel {
  Q_OBJECT

public:
  VideoModel(QObject* parent = 0);
  ~VideoModel();

  enum Role {
    Role_Name = Qt::UserRole + 1,
    Role_ID,
    Role_Type,

    Role_IsContentsLoaded,
  };

  void Reset();

  // QAbstractItemModel
  bool hasChildren(const QModelIndex& parent) const;
  bool canFetchMore(const QModelIndex& parent) const;
  void fetchMore(const QModelIndex& parent);

private:
  void Browse(const QString& parent_id);

private slots:
  void BrowseRequestFinished();

private:
  QScopedPointer<AVClient> client_;

  QMap<QString, QStandardItem*> items_by_id_;
  QMap<QNetworkReply*, QString> browse_requests_;
};

#endif // VIDEOMODEL_H

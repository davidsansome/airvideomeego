#ifndef VIDEOMODEL_H
#define VIDEOMODEL_H

#include <QScopedPointer>
#include <QStandardItemModel>

class AVClient;

class QNetworkReply;


class VideoModel : public QStandardItemModel {
  Q_OBJECT
  Q_ENUMS(Role)
  Q_ENUMS(Type)
  Q_PROPERTY(QString model_id READ model_id CONSTANT)
  Q_PROPERTY(QString thumbnail_provider_name READ thumbnail_provider_name CONSTANT)

public:
  VideoModel(QObject* parent = 0);
  ~VideoModel();

  static const int kMaxAttempts;
  static const int kRetryDelayMs;

  enum Role {
    Role_Name = Qt::UserRole + 1,
    Role_ID,
    Role_Type,

    Role_CurrentAttempt,
    Role_IsContentsLoaded,

    // For video only
    Role_IsVideoValid,
    Role_FileSize,
    Role_Duration,
    Role_ThumbnailData,
  };

  enum Type {
    Type_Unknown = 0,
    Type_Folder,
    Type_Video,
  };

  void Reset();

  QString model_id() const;
  QString thumbnail_provider_name() const;

  QByteArray ThumbnailData(const QString& item_id) const;

  // QAbstractItemModel
  bool hasChildren(const QModelIndex& parent) const;
  bool canFetchMore(const QModelIndex& parent) const;
  void fetchMore(const QModelIndex& parent);

  // QObject
  void timerEvent(QTimerEvent* e);

private:
  void Browse(const QString& parent_id);

private slots:
  void BrowseRequestFinished();
  void GetMediaInfoRequestFinished();

private:
  QScopedPointer<AVClient> client_;

  QMap<QString, QStandardItem*> items_by_id_;
  QMap<QNetworkReply*, QString> browse_requests_;
  QMap<QNetworkReply*, QString> media_info_requests_;

  QMap<int, QString> media_info_retries_;
};

#endif // VIDEOMODEL_H

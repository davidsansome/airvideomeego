#ifndef VIDEOMODELTHUMBNAILPROVIDER_H
#define VIDEOMODELTHUMBNAILPROVIDER_H

#include <QDeclarativeImageProvider>

class VideoModel;

class VideoModelThumbnailProvider : public QDeclarativeImageProvider {
public:
  VideoModelThumbnailProvider(VideoModel* model);

  static const int kMaxWidth;
  static const int kMaxHeight;

  QImage requestImage(const QString& id, QSize* size, const QSize& requested_size);

private:
  VideoModel* model_;
};

#endif // VIDEOMODELTHUMBNAILPROVIDER_H

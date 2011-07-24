#include "videomodel.h"
#include "videomodelthumbnailprovider.h"

#include <QtDebug>

const int VideoModelThumbnailProvider::kMaxWidth = 144; // 16:9
const int VideoModelThumbnailProvider::kMaxHeight = 81;

VideoModelThumbnailProvider::VideoModelThumbnailProvider(VideoModel* model)
  : QDeclarativeImageProvider(Image),
    model_(model)
{
}

QImage VideoModelThumbnailProvider::requestImage(
    const QString& id, QSize* size, const QSize& requested_size) {
  QByteArray data(model_->ThumbnailData(id));
  QImage image = QImage::fromData(data);

  if (!image.isNull()) {
    // Ignore the requested size since it's usually cock.
    if (image.width() > kMaxWidth || image.height() > kMaxHeight) {
      image = image.scaled(QSize(kMaxWidth, kMaxHeight), Qt::KeepAspectRatio);
    }
    if (size) {
      *size = image.size();
    }
  }

  return image;
}



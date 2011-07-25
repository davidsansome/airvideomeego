#include "videomodel.h"
#include "videomodelthumbnailprovider.h"

#include <QtDebug>

const int VideoModelThumbnailProvider::kMaxWidth = 155; // 16:9 ish
const int VideoModelThumbnailProvider::kMaxHeight = 87;

VideoModelThumbnailProvider::VideoModelThumbnailProvider(VideoModel* model)
  : QDeclarativeImageProvider(Image),
    model_(model)
{
}

QImage VideoModelThumbnailProvider::requestImage(
    const QString& id, QSize* size, const QSize&) {
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



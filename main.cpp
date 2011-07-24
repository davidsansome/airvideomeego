#include "videomodel.h"
#include "videomodelthumbnailprovider.h"

#include <QtGui/QApplication>
#include <QtDeclarative>


int main(int argc, char** argv) {
  QApplication app(argc, argv);

  qmlRegisterType<VideoModel>("AirVideo", 1, 0, "VideoModel");

  VideoModel model;
  VideoModelThumbnailProvider* thumbnail_provider =
      new VideoModelThumbnailProvider(&model);

  QDeclarativeView view;
  view.rootContext()->setContextProperty("video_model", &model);
  view.engine()->addImageProvider(model.thumbnail_provider_name(), thumbnail_provider);

  view.setSource(QUrl("qrc:/qml/main.qml"));
  view.showFullScreen();

  return app.exec();
}

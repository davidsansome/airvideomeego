#include "videomodel.h"

#include <QtGui/QApplication>
#include <QtDeclarative>


int main(int argc, char** argv) {
  QApplication app(argc, argv);

  VideoModel model;

  QDeclarativeView view;
  view.rootContext()->setContextProperty("video_model", &model);
  view.setSource(QUrl("qrc:/qml/main.qml"));
  view.showFullScreen();

  return app.exec();
}

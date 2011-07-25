import QtQuick 1.1
import com.meego 1.0

import AirVideo 1.0
import "utilities.js" as Utilities

Page {
  id: browse_page

  property alias root_index: visual_video_model.rootIndex

  tools: ToolBarLayout {
    visible: true

    ToolIcon {
      platformIconId: "toolbar-back";
      onClicked: pageStack.pop();
    }

    ToolIcon {
      platformIconId: "toolbar-view-menu";
      onClicked: (myMenu.status == DialogStatus.Closed) ? myMenu.open() : myMenu.close()
    }
  }

  VisualDataModel {
    id: visual_video_model
    model: video_model

    delegate: Item {
      height: 88
      width: parent.width

      // The thumbnail or folder icon
      Image {
        id: icon
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter

        width: type == VideoModel.Type_Video ? 155 : undefined
        fillMode: Image.PreserveAspectFit

        source: (type == VideoModel.Type_Folder)
                  ? "image://theme/icon-m-common-directory"
                  : (is_contents_loaded)
                      ? "image://" + video_model.thumbnail_provider_name + "/" + id
                      : "image://theme/icon-m-content-video"
        asynchronous: true
      }

      // The item name and information
      Column {
        anchors.left: icon.right
        anchors.leftMargin: 6
        anchors.right: right_icon.left
        anchors.rightMargin: 6
        anchors.verticalCenter: parent.verticalCenter

        // Item name
        Text {
          text: name
          width: parent.width

          font.family: "Nokia Pure Text"
          font.pixelSize: 26
          font.bold: true
          elide: Text.ElideRight
        }

        // Item information
        Text {
          text: is_contents_loaded
                  ? is_video_valid
                    ? Utilities.pretty_duration(duration) + ", " +
                      Utilities.pretty_size(file_size)
                    : "Error loading video"
                  : "Loading..."

          visible: type == VideoModel.Type_Video

          font.pixelSize: 20
          color: "#cc6633"
        }
      }

      Image {
        id: right_icon
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        source: "image://theme/icon-m-common-drilldown-arrow" + (theme.inverted ? "-inverse" : "")
      }

      MouseArea {
        anchors.fill: parent
        onClicked: {
          pageStack.push(Qt.resolvedUrl("BrowsePage.qml"), {
            root_index: visual_video_model.modelIndex(index)
          })
        }
      }
    }
  }

  ListView {
    model: visual_video_model
    anchors.fill: parent
  }
}

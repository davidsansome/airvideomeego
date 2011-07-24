import QtQuick 1.1
import com.meego 1.0

Page {
  id: browse_page

  property alias root_index: visual_video_model.rootIndex

  tools: ToolBarLayout {
    visible: true

    ToolIcon {
      platformIconId: "toolbar-back";
      onClicked: {
        pageStack.pop();
      }
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

      Row {
        anchors.fill: parent

        Text {
          anchors.verticalCenter: parent.verticalCenter
          font.family: "Nokia Pure Text"
          font.pixelSize: 26
          font.bold: true
          text: model.display
        }
      }

      Image {
        source: "image://theme/icon-m-common-drilldown-arrow" + (theme.inverted ? "-inverse" : "")
        anchors.right: parent.right;
        anchors.verticalCenter: parent.verticalCenter
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

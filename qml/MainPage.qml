import QtQuick 1.1
import com.meego 1.0

Page {
  id: mainPage
  tools: ToolBarLayout {
    visible: true

    ToolIcon {
      platformIconId: "toolbar-back";
      onClicked: {
        visual_video_model.rootIndex = visual_video_model.parentModelIndex()
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

      MouseArea {
        anchors.fill: parent
        onClicked: {
          console.log("Setting new index to " + index)
          console.log(visual_video_model.modelIndex(index));
          visual_video_model.rootIndex = visual_video_model.modelIndex(index)
        }
      }
    }
  }

  ListView {
    model: visual_video_model
    anchors.fill: parent
  }
}

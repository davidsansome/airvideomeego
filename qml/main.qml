import QtQuick 1.1
import com.meego 1.0

PageStackWindow {
  id: appWindow

  initialPage: mainPage

  MainPage{id: mainPage}

  Menu {
    id: myMenu
    visualParent: pageStack
    MenuLayout {
      MenuItem { text: "Sample menu item" }
    }
  }
}

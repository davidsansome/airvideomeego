import QtQuick 1.1
import com.meego 1.0

PageStackWindow {
  id: appWindow

  initialPage: main_browse_page

  BrowsePage {id: main_browse_page}

  Menu {
    id: myMenu
    visualParent: pageStack
    MenuLayout {
      MenuItem { text: "Sample menu item" }
    }
  }
}

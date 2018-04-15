import QtQuick 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4

Item {
    ComboBox {
       id: fontBox
       currentIndex: 0
       activeFocusOnPress: true
       width: 176
       anchors.top: parent.top
       anchors.topMargin: 200
       anchors.horizontalCenter: parent.horizontalCenter

       Image {
           width: 12
           height: 8
           anchors.right: parent.right
           anchors.rightMargin: 8
           anchors.verticalCenter: parent.verticalCenter

           source: "qrc:/Resource/combo_arrow.png"

       }

       style: ComboBoxStyle {
           background: Rectangle {     //box style (not drop-down part)
    //                            implicitWidth: 176
    //                            implicitHeight: 24
               width: parent.width
               height: parent.height
               radius: 2
               color: "#f9f9f9"
               border.color: fontBox.hovered ? "light blue" : "transparent"
           }

           label: Text {
               text: control.currentText
               font.family: mainFont.name
               font.pointSize: 7
           }

           //drop-down customization
           property Component __dropDownStyle: MenuStyle {
               __maxPopupHeight: 120
               __menuItemType: "comboboxitem"

               frame: Rectangle {      //drop-down box style
                   color: "#f9f9f9"
                   width: 174
                   radius: 2
               }
               itemDelegate.label:     //item text
                   Text {
                       text: styleData.text
                       font.family: mainFont.name
                       font.pointSize: 7
                       color: styleData.selected ? "#666666" : "#303030"
    //                                    color: styleData.selected ? "red" : "blue"
                   }
               itemDelegate.background: Rectangle {
                   color: styleData.selected ? "#eaeaea" : "#f9f9f9"
               }


               //scroller customization
               __scrollerStyle: ScrollViewStyle {
                   scrollBarBackground: Rectangle {
                       color: "#eaeaea"
                       implicitWidth: 7
                       implicitHeight: 110
                   }
                   handle: Rectangle {
                       color: "#b7b7b7"
                       implicitWidth: 9
                       implicitHeight: 45
                       radius: 2
                   }
                   minimumHandleLength: 35
                   incrementControl: Rectangle {
                       implicitWidth: 0
                       implicitHeight: 0
                   }
                   decrementControl: Rectangle {
                       implicitWidth: 0
                       implicitHeight: 0
                   }
               }
           }
       }
    }
}

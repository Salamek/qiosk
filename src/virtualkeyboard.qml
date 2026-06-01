import QtQuick 2.12
import QtQuick.VirtualKeyboard 2.2

Item {
    id: root

    InputPanel {
        id: inputPanel
        objectName: "inputPanel"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }
}

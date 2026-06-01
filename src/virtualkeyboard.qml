import QtQuick 2.12
import QtQuick.VirtualKeyboard 2.2

Item {
    id: root

    InputPanel {
        id: inputPanel
        objectName: "inputPanel"
        anchors.left: parent.left
        anchors.right: parent.right
        y: Qt.inputMethod.visible ? parent.height - inputPanel.height : parent.height
    }
}

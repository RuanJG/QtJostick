import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

Item {
    width: 640
    height: 480

    property alias buttonFly: buttonFly
    property alias buttonArm: buttonArm
    property alias buttonConnect: buttonConnect
    property alias qgcDebugConsole: qgcDebugConsole
    property alias qgcStatus: qgcStatus
    property alias qgcSeriolBox: qgcSeriolBox
    property alias qgcModeList: qgcModeList
    property alias  buttonParam: buttonParam


    RowLayout {
        anchors.centerIn: parent

        Button {
            id: buttonConnect
            text: qsTr("Connect")
        }

        Button {
            id: buttonArm
            text: qsTr("Armed")
            enabled: false
        }

        Button {
            id: buttonFly
            text: qsTr("Fly Up")
            enabled: false
        }
    }
    ComboBox {
        id : qgcSeriolBox
        editable: false
    }
    ComboBox {
        id : qgcModeList
        editable: false
        x: 40
        y: 50
        visible: false

    }
    TextArea {
        id: qgcDebugConsole
        x: 27
        y: 314
        width: 605
        height: 150
        activeFocusOnPress: false
        readOnly: true
    }

    Text {
        id: qgcStatus
        x: 507
        y: 41
        width: 87
        height: 29
        text: qsTr("Disconnect")
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 25
    }

    Button {
        id: buttonParam
        x: 450
        y: 229
        width: 82
        height: 23
        text: qsTr("update Param")
        checkable: true
        enabled: false
    }
}

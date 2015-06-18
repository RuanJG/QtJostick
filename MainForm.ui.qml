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

    property alias qgcThrLable:qgcThrLable
    property alias qgcYawLable:qgcYawLable
    property alias qgcRollLable:qgcRollLable
    property alias qgcPitchLable:qgcPitchLable

    property alias qgcYawSlider: qgcYawSlider
    property alias qgcThrSlider: qgcThrSlider
    property alias qgcRollSlider: qgcRollSlider
    property alias qgcPitchSlider: qgcPitchSlider

    property alias qgcRcSteupPersen: qgcRcSteupPersen
    property alias qgcRcTrimPersen: qgcRcTrimPersen

    property alias buttonSendRc: buttonSendRc

    property alias buttonRcSetParam: buttonRcSetParam



    RowLayout {
        anchors.verticalCenterOffset: 35
        anchors.horizontalCenterOffset: -24
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
        x: 27
        y: 263
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
        x: 419
        y: 263
        width: 82
        height: 23
        text: qsTr("update Param")
        checkable: true
        enabled: false
    }

    Slider {
        id:  qgcYawSlider
        x: 50
        y: 157
        stepSize: 1
        maximumValue: 20
        value: 10
    }

    Slider {
        id: qgcThrSlider
        x: 139
        y: 85
        width: 22
        height: 167
        stepSize: 1
        maximumValue: 20
        value: 10
        orientation: Qt.Vertical
    }

    Slider {
        id: qgcPitchSlider
        x: 368
        y: 85
        width: 22
        height: 167
        minimumValue: 1000
        updateValueWhileDragging: false
        stepSize: 1
        maximumValue: 2000
        tickmarksEnabled: false
        value: 1500
        orientation: Qt.Vertical
    }

    Slider {
        id: qgcRollSlider
        x: 279
        y: 157
        minimumValue: 1000
        stepSize: 1
        maximumValue: 2000
        value: 1500
    }

    Text {
        id: qgcYawLable
        x: 50
        y: 127
        width: 39
        height: 19
        text: qsTr("0")
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 13
    }

    Text {
        id: qgcThrLable
        x: 167
        y: 85
        width: 39
        height: 19
        text: qsTr("0")
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 13
        horizontalAlignment: Text.AlignHCenter
    }

    Text {
        id: qgcRollLable
        x: 279
        y: 127
        width: 39
        height: 19
        text: qsTr("0")
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 13
        horizontalAlignment: Text.AlignHCenter
    }

    Text {
        id: qgcPitchLable
        x: 405
        y: 85
        width: 39
        height: 19
        text: qsTr("0")
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 13
        horizontalAlignment: Text.AlignHCenter
    }

    Text {
        id: textsteup
        x: 507
        y: 136
        width: 31
        height: 20
        text: qsTr("steup")
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 12
    }

    Text {
        id: textTrim
        x: 507
        y: 172
        width: 31
        height: 20
        text: qsTr("trim")
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 12
    }

    Button {
        id: buttonSendRc
        x: 526
        y: 263
        text: qsTr("Send Rc")
        enabled: false
    }

    TextField {
        id: qgcRcTrimPersen
        x: 564
        y: 172
        width: 43
        height: 20
        text: "10"
        placeholderText: qsTr("Text Field")
    }

    TextField {
        id: qgcRcSteupPersen
        x: 564
        y: 136
        width: 43
        height: 20
        text: "40"
        placeholderText: qsTr("Text Field")
    }

    Button {
        id: buttonRcSetParam
        x: 526
        y: 218
        text: qsTr("update Steup")
    }
}

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
        width: 549
        height: 23
        visible: true
        id: buttonsRow
        anchors.verticalCenterOffset: 58
        anchors.horizontalCenterOffset: -18
        anchors.centerIn: parent

        ComboBox {
            id : qgcModeList
            editable: false
            //x: 27
            //y: 263
            visible: true
        }
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
        Button {
            id: buttonParam
            //x: 439
            //y: 263
            //width: 82
            //height: 23
            text: qsTr("update Param")
            checkable: true
            enabled: false
        }
        Button {
            id: buttonSendRc
            //x: 526
            //y: 263
            text: qsTr("Send Rc")
            enabled: false
        }

    }
    ComboBox {
        id : qgcSeriolBox
        width: 125
        height: 24
        anchors.verticalCenterOffset: -223
        anchors.horizontalCenterOffset: -249
        anchors.centerIn: parent
        editable: false
    }

    TextArea {
        id: qgcDebugConsole
        x: 27
        y: 331
        width: 605
        height: 133
        activeFocusOnPress: false
        readOnly: true

        anchors.verticalCenterOffset: 158
        anchors.horizontalCenterOffset: 1
        anchors.centerIn: parent
    }

    Text {
        id: qgcStatus
        x: 481
        y: 0
        width: 140
        height: 34
        anchors.verticalCenterOffset: -215
        anchors.horizontalCenterOffset: 242
        anchors.centerIn: parent

        text: qsTr("Disconnect")
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 25
    }




    Slider {
        id: qgcThrSlider
        x: rcRow.x+qgcYawSlider.x+(qgcYawSlider.width/2)-width/2
        y: rcRow.y+qgcYawSlider.y-(height/2)+width/2
        width: 22
        height: 167
        clip: false
        stepSize: 1
        maximumValue: 20
        value: 0
        orientation: Qt.Vertical
    }

    Slider {
        id: qgcPitchSlider
        x: rcRow.x+qgcRollSlider.x+(qgcRollSlider.width/2)-width/2
        y: rcRow.y+qgcRollSlider.y-(height/2)+width/2
        width: 22
        height: 167
        minimumValue: 1000
        updateValueWhileDragging: false
        stepSize: 1
        maximumValue: 2000
        tickmarksEnabled: false
        value: 1000
        orientation: Qt.Vertical
    }



    Text {
        id: qgcYawLable
        x: rcRow.x+qgcYawSlider.x
        y: rcRow.y+qgcYawSlider.y-30
        width: 39
        height: 19
        text: qsTr("0")
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 13
    }

    Text {
        id: qgcThrLable
        x: qgcThrSlider.x+20
        y: qgcThrSlider.y
        width: 39
        height: 19
        text: qsTr("0")
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 13
        horizontalAlignment: Text.AlignHCenter
    }

    Text {
        id: qgcRollLable
        x: rcRow.x+qgcRollSlider.x
        y: rcRow.y+qgcRollSlider.y-30
        width: 39
        height: 19
        text: qsTr("0")
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 13
        horizontalAlignment: Text.AlignHCenter
    }

    Text {
        id: qgcPitchLable
        x: qgcPitchSlider.x+20
        y: qgcPitchSlider.y
        width: 39
        height: 19
        text: qsTr("0")
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 13
        horizontalAlignment: Text.AlignHCenter
    }

    TextField {
        id: qgcRcSteupPersen
        x: settingRow.x+textsteup.x+50
        y: settingRow.y+textsteup.y
        width: 30
        text: "40"
        placeholderText: qsTr("Text Field")
    }
    TextField {
        id: qgcRcTrimPersen
        x: settingRow.x+textTrim.x+50
        y: settingRow.y+textTrim.y
        width: 30
        text: "10"
        placeholderText: qsTr("Text Field")
    }
    ColumnLayout {
        id: settingRow
        width: 83
        height: 175
        visible: true

        anchors.verticalCenterOffset: -67
        anchors.horizontalCenterOffset: 224
        anchors.centerIn: parent

        Text {
            id: textsteup
            text: qsTr("steup")
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 12
        }
        Text {
            id: textTrim
            text: qsTr("trim")
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 12
        }
        Button {
            id: buttonRcSetParam
            text: qsTr("update Steup")
        }
    }
    RowLayout {
        width: 451
        height: 23
        visible: true
        id: rcRow
        anchors.verticalCenterOffset: -67
        anchors.horizontalCenterOffset: -65
        anchors.centerIn: parent

        Slider {
            id:  qgcYawSlider
            stepSize: 1
            maximumValue: 20
            value: 10
        }
        Slider {
            id: qgcRollSlider
            minimumValue: 1000
            stepSize: 1
            maximumValue: 2000
            value: 1500
        }

    }



}

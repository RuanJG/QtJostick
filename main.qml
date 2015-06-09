import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QgcUi 1.0

ApplicationWindow {
    title: qsTr("Hello World")
    width: 640
    height: 480
    visible: true

    menuBar: MenuBar {
        /*
        Menu {
            title: qsTr("&File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: messageDialog.show(qsTr("Open action triggered"));
            }
            MenuItem {
                text: qsTr("E&xit")
                onTriggered: Qt.quit();
            }
        }
        */
    }
    MessageDialog {
        id: messageDialog
        title: qsTr("Message")

        function show(caption) {
            messageDialog.text = caption;
            messageDialog.open();
        }
        function testShow(newtitle,newcontent){
            messageDialog.title = newtitle;
            messageDialog.text = newcontent;
            messageDialog.open();

        }
        onAccepted: {
            view.button1.text = qsTr("passed me");

        }
    }


    MainForm {
        anchors.fill: parent
        id:view

        // qgcCore connect ui by QgcUi
        QgcUi {
            id: qgcui
            //debugmsg
            onHasQgcDebugMsg:{
                view.qgcDebugConsole.append("qgc: "+qgcui.debugmsg)
            }
        }

        // msg output to the controler

        buttonArm.onClicked: debugMsg(qsTr("Button 1 pressed"))
        buttonConnect.onClicked: qgcui.debug(qsTr("Button 2 pressed"))
        buttonFly.onClicked: messageDialog.show(qsTr("Button 3 pressed"))

        focus:true
        Keys.onPressed: {
            if (event.key == Qt.Key_A) {

                event.accepted = true;
            }
            debugMsg("Key "+event.key+" was pressed");
        }

        function debugMsg(msg){
            qgcDebugConsole.append("UI: "+msg)
        }







        //msg get from controler

    }

    /*MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
           messageDialog.show1("Mouse",qsTr("click in "+mouseX+","+mouseY))
        }
    }*/

}

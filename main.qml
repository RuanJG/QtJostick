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
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: 0

        // qgcCore connect ui by QgcUi
        QgcUi {
            id: qgcui
            //debugmsg
            onHasQgcDebugMsg:{
                view.qgcDebugConsole.append("qgc: "+qgcui.debugmsg)
            }
            onQgcStatusChange:{
                view.qgcStatus.text=qgcui.status;
                if(qgcui.status == "Connect"){
                    view.buttonConnect.text = "DisConnect";
                }else{
                    view.buttonConnect.text = "Connect";
                }
            }

        }


        function debugMsg(msg){
            qgcDebugConsole.append("UI: "+msg)
        }



        //********************* ui to qgc

        //button event
        buttonArm.onClicked:{
            qgcui.qgcArm();
        }
        buttonConnect.onClicked: {
            if(qgcui.status == "DisConnect"){
                qgcui.qgcConnect();
            }else
                qgcui.qgcDisconnect();
        }
        buttonFly.onClicked: qgcui.qgcFly()
        //keyboard event
        focus:true
        Keys.onPressed: {
            if (event.key == Qt.Key_A) {
                event.accepted = true;
                qgcDebugConsole.remove(0,2000);
            }

            debugMsg("Key "+event.key+" was pressed");

        }
        //seriol select event
        ListModel {
            id: portlist
            ListElement { text: "None" }
        }
        qgcSeriolBox.model: portlist
        qgcSeriolBox.onCurrentTextChanged: {
            debugMsg("seriol select "+qgcSeriolBox.currentText);
            qgcui.seriolport = qgcSeriolBox.currentText;
            updateseriollist();

        }
        function updateseriollist()
        {
            var list = new Array();
            var i;
            list = qgcui.getSeriolPortList();
            portlist.clear();
            for(i=0; i< list.length ; i++){
                portlist.append({text: list[i]});
            }
        }




        //********************** qgc to ui











        Component.onCompleted:
        {
            updateseriollist();
            view.qgcStatus.text=qgcui.status;

        }




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

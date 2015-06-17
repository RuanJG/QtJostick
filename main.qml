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

                if( qgcui.isConnect() ){
                    view.buttonConnect.text = "DisConnect" ;
                    view.qgcStatus.text = "Connect";

                    view.qgcPitchSlider.maximumValue = qgcui.getPitchMax();
                    view.qgcPitchSlider.minimumValue = qgcui.getPitchMin();
                    view.qgcRollSlider.maximumValue = qgcui.getRollMax();
                    view.qgcRollSlider.minimumValue = qgcui.getRollMin();
                    view.qgcYawSlider.maximumValue = qgcui.getYawMax();
                    view.qgcYawSlider.minimumValue = qgcui.getYawMin();
                    view.qgcThrSlider.maximumValue = qgcui.getThrMax();
                    view.qgcThrSlider.minimumValue = qgcui.getThrMin();


                }else{
                    view.buttonConnect.text = "Connect";
                    view.qgcStatus.text = "DisConnect";
                }
                view.buttonArm.text = qgcui.isArmed()?"DisArmed":"Armed";
                view.qgcModeList.currentIndex = getCopterMode();
                view.qgcModeList.visible = qgcui.isConnect();
                view.buttonFly.enabled = qgcui.isConnect();
                view.buttonArm.enabled = qgcui.isConnect();
                view.buttonParam.enabled = qgcui.isConnect();
            }
            onQgcRcChange:{
                view.qgcYawLable.text = qgcui.getYaw();
                view.qgcPitchLable.text = qgcui.getPitch();
                view.qgcRollLable.text = qgcui.getRoll();
                view.qgcThrLable.text = qgcui.getThr();


                view.qgcPitchSlider.value = qgcui.getPitch();
                view.qgcRollSlider.value = qgcui.getRoll();
                view.qgcThrSlider.value = qgcui.getThr();
                view.qgcYawSlider.value = qgcui.getYaw();
            }

        }


        function debugMsg(msg){
            qgcDebugConsole.append("UI: "+msg)
        }



        //********************* ui to qgc

        //button event
        buttonArm.onClicked:{
            if( qgcui.isArmed() )
                qgcui.qgcDisArm();
            else
                qgcui.qgcArm();
        }
        buttonConnect.onClicked: {
            if(! qgcui.isConnect()){
                qgcui.qgcConnect();
            }else
                qgcui.qgcDisconnect();
        }
        buttonFly.onClicked: qgcui.qgcFly()
        buttonParam.onClicked: {
            qgcui.updateParam();
        }

        //keyboard event
        focus:true
        Keys.onPressed: {
            qgcui.qgcGetKey(event.key);
            event.accepted = true;
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

        // mode Selection
        ListModel {
            id: modelist
            ListElement {name:"STABILIZE"}
            ListElement {name:"ACRO"}
            ListElement {name:"ALT_HOLD"}
            ListElement {name:"AUTO"}
            ListElement {name:"GUIDED"}
            ListElement {name:"LOITER"}
            ListElement {name:"RTL"}
            ListElement {name:"CIRCLE"}
            ListElement {name:"LAND"}
            ListElement {name:"OF_LOITER"}

        }
        qgcModeList.model: modelist
        qgcModeList.onCurrentIndexChanged: {
            debugMsg("Now use mode="+qgcModeList.currentIndex);
            qgcui.qgcSetMode(qgcModeList.currentIndex);
        }




        //********************** qgc to ui











        Component.onCompleted:
        {
            updateseriollist();

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

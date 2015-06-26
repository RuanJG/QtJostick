import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QgcUi 1.0

ApplicationWindow {
    title: qsTr("Hello World")
    width: 828
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
                //view.qgcModeList.visible = qgcui.isConnect();
                view.buttonFly.enabled = qgcui.isConnect();
                view.buttonArm.enabled = qgcui.isConnect();
                view.buttonParam.enabled = qgcui.isConnect();
                view.buttonSendRc.enabled = qgcui.isConnect();
                view.qgcModeList.enabled = qgcui.isConnect();

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

                view.qgcCameraPitchSlider.value = qgcui.getRcValue(view.cameraPitchChannelBox.currentIndex+5);
                view.qgcCameraRollSlider.value = qgcui.getRcValue(view.cameraRollChannelBox.currentIndex+5);

                view.qgcRcSteupPersen.text = qgcui.getRcSteup();
                view.qgcRcTrimPersen.text = qgcui.getRcTrim();

                if( view.qgcDebugConsole.lineCount > 200 ){
                    view.qgcDebugConsole.remove(0, view.qgcDebugConsole.length/2 );
                }
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
        buttonSendRc.onClicked: {
            var sp = view.qgcRcSteupPersen.text;
            var tp = view.qgcRcTrimPersen.text;
            if( qgcui.updateRcParam(sp,tp) )
                qgcui.qgcSendRc();

            updateRcMask();

            if( qgcui.isSendingRc())
                buttonSendRc.text = "Stop RC";
            else{
                updateRcMask();
                buttonSendRc.text = "Send RC";
            }

        }
        buttonParam.onClicked: {
            qgcui.updateParam();
        }

        buttonRcSetParam.onClicked: {
            var sp = view.qgcRcSteupPersen.text;
            var tp = view.qgcRcTrimPersen.text;
            qgcui.updateRcParam(sp,tp);
            view.qgcRcSteupPersen.text = qgcui.getRcSteup();
            view.qgcRcTrimPersen.text = qgcui.getRcTrim();
            view.focus=true;
        }

        //keyboard event
        focus:true
        Keys.onPressed: {
            if( qgcRcSteupPersen.focus || qgcRcTrimPersen.focus )
                return;
            qgcui.qgcGetKey(event.key);
            event.accepted = true;
        }
        Keys.onReleased: {
            if( qgcRcSteupPersen.focus || qgcRcTrimPersen.focus )
                return;
            qgcui.qgcReleaseKey(event.key);
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
        ListModel {
            id: baudlist
            ListElement {name:"57600"}
            ListElement {name:"115200"}
        }
        qgcSeriolBaudBox.model: baudlist
        qgcSeriolBaudBox.onCurrentTextChanged: {
            qgcui.setBaud(57600+57600*qgcSeriolBaudBox.currentIndex);
        }
        ListModel {
            id: auxRclist
            ListElement {name:"5"}
            ListElement {name:"6"}
            ListElement {name:"7"}
            ListElement {name:"8"}
        }
        cameraRollChannelBox.model:auxRclist
        cameraPitchChannelBox.model:auxRclist

        qgcCameraPitchSlider.onValueChanged: {
            if( qgcui.isConnect() )
                qgcui.setRcValue(cameraPitchChannelBox.currentIndex+5,qgcCameraPitchSlider.value);
        }
        qgcCameraRollSlider.onValueChanged: {
            if( qgcui.isConnect() )
                qgcui.setRcValue(cameraRollChannelBox.currentIndex+5,qgcCameraRollSlider.value);
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


        cameraCheckBox.onCheckedChanged: {
            if( cameraCheckBox.checked ){
                debugMsg("camera checked");
            }else{
                debugMsg("camera does not checked");
            }
            qgcui.setRcMask(cameraPitchChannelBox.currentIndex+5,cameraCheckBox.checked);
            qgcui.setRcMask(cameraRollChannelBox.currentIndex+5,cameraCheckBox.checked);

            cameraPitchChannelBox.enabled = !cameraCheckBox.checked;
            cameraRollChannelBox.enabled = !cameraCheckBox.checked

        }
        rcCheckBox.onCheckedChanged: {
            if( rcCheckBox.checked ){
                debugMsg("rcCheckBox checked");
            }else{
                debugMsg("rcCheckBox does not checked");
            }
            qgcui.setRcMask(1,rcCheckBox.checked);
            qgcui.setRcMask(2,rcCheckBox.checked);
            qgcui.setRcMask(3,rcCheckBox.checked);
            qgcui.setRcMask(4,rcCheckBox.checked);
        }
        function updateRcMask()
        {
            qgcui.setRcMask(1,rcCheckBox.checked);
            qgcui.setRcMask(2,rcCheckBox.checked);
            qgcui.setRcMask(3,rcCheckBox.checked);
            qgcui.setRcMask(4,rcCheckBox.checked);
            qgcui.setRcMask(cameraPitchChannelBox.currentIndex+5,cameraCheckBox.checked);
            qgcui.setRcMask(cameraRollChannelBox.currentIndex+5,cameraCheckBox.checked);
        }



        //********************** qgc to ui











        Component.onCompleted:
        {
            view.qgcRcSteupPersen.text = qgcui.getRcSteup();
            view.qgcRcTrimPersen.text = qgcui.getRcTrim();
            updateseriollist();
            view.qgcModeList.enabled = false;

            view.qgcCameraPitchSlider.maximumValue = 2000;//qgcui.getRcParamMaxValue(view.cameraPitchChannelBox.currentIndex+5);
            view.qgcCameraPitchSlider.minimumValue = 1000;//qgcui.getRcParamMinValue(view.cameraPitchChannelBox.currentIndex+5);
            view.qgcCameraRollSlider.maximumValue = 2000;//qgcui.getRcParamMaxValue(view.cameraRollChannelBox.currentIndex+5);
            view.qgcCameraRollSlider.minimumValue =  1000;//qgcui.getRcParamMinValue(view.cameraRollChannelBox.currentIndex+5);

            view.cameraCheckBox.checked = false;
            cameraPitchChannelBox.currentIndex = 1;
            cameraRollChannelBox.currentIndex = 2;
            cameraPitchChannelBox.enabled = !cameraCheckBox.checked;
            cameraRollChannelBox.enabled = !cameraCheckBox.checked

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

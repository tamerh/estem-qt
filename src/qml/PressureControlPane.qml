import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.0

import org.example.ufcs 1.0 // for the Style singleton

Item {
    id: control

    height: column.implicitHeight

    property bool collapsed: true

    Column {
        id: column
        anchors.fill: parent

        RowLayout {
            id: rowLayout
            anchors.left: parent.left
            anchors.right: parent.right

            Label {
                id: pressureLabel
                text: qsTr("Pressure")
                Layout.rightMargin: 100
                Layout.fillWidth: true
                font.pointSize: Style.heading1.fontSize

            }

            Label {
                id: label1
                text: qsTr("Control")
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                visible: control.collapsed
            }

            Label {
                id: label2
                text: pressureControllerControl.pv + " PSI"
                Layout.rightMargin: 15
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                visible: control.collapsed
            }

            Label {
                id: label3
                text: qsTr("Flow")
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                visible: control.collapsed
            }

            Label {
                id: label4
                text: pressureControllerFlow.pv + " PSI"
                Layout.rightMargin: 15
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                visible: control.collapsed
            }

            Button {
                id: showHideButton
                text: collapsed ? qsTr("Show") : qsTr("Hide")
                Layout.leftMargin: 20
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                flat: true
                onClicked: { control.collapsed = !control.collapsed }
            }

        }

        RowLayout {
            id: pressureControllerRow
            anchors.left: parent.left
            anchors.right: parent.right
            height: control.collapsed ? 0 : implicitHeight

            Behavior on height {
                NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
            }

            Column {
                Layout.alignment: Qt.AlignHCenter
                visible: !control.collapsed

                Label {
                    text : "Control layer"
                    font.pointSize: Style.heading2.fontSize
                    bottomPadding: 10
                    anchors.right: parent.right
                    anchors.left: parent.left
                    horizontalAlignment: Text.AlignHCenter
                }

                PressureController {
                    id: pressureControllerControl
                    controllerNumber: 1
                    minPressure: 0
                    maxPressure: 29.5
                }
            }

            Column {
                Layout.alignment: Qt.AlignHCenter
                visible: !control.collapsed

                Label {
                    text : "Flow layer"
                    font.pointSize: Style.heading2.fontSize
                    bottomPadding: 10
                    anchors.right: parent.right
                    anchors.left: parent.left
                    horizontalAlignment: Text.AlignHCenter
                }

                PressureController {
                    id: pressureControllerFlow
                    controllerNumber: 2
                    minPressure: 0
                    maxPressure: 4.8
                }

            }
        }
    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:11;flowX:-216;flowY:167}D{i:12;flowX:-207;flowY:173}
D{i:13;flowX:-191;flowY:177}D{i:14;flowX:-191;flowY:188}D{i:15;flowX:-196;flowY:116}
D{i:16;flowX:-214;flowY:148}D{i:10;anchors_x:8;flowX:"-207";flowY:173}D{i:18;flowX:-212;flowY:0}
D{i:19;flowX:-195;flowY:1}D{i:9;anchors_height:100;anchors_width:100}D{i:1;anchors_x:8}
D{i:3;flowX:"-207";flowY:173}D{i:4;flowX:"-191";flowY:177}D{i:5;flowX:"-191";flowY:188}
D{i:6;flowX:"-196";flowY:116}D{i:7;flowX:"-214";flowY:148}D{i:2;flowX:"-216";flowY:167}
}
##^##*/

import QtQuick 2.12
import QtQuick.Controls 2.12

import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12

import org.example.ufcs 1.0 // for the Style singleton

Item {
    Flickable {
        anchors.fill: parent
        contentHeight: topLevelColumnLayout.height
        ScrollBar.vertical: ScrollBar {}

        ColumnLayout {
            id: topLevelColumnLayout
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: Style.view.margin
            spacing: 10
            Component.onCompleted: {
                //Backend.connect()
            }

            Label {
                text: qsTr("Valves")
                font.pointSize: Style.heading1.fontSize
                padding: Style.heading1.padding
                leftPadding: Style.heading1.paddingLeft
                topPadding: 0
                Layout.alignment: Qt.AlignTop
            }


            GridLayout {
                id: controlLayerValveGrid
                Layout.fillWidth: true
                Layout.preferredWidth: parent.width
                Layout.alignment: Qt.AlignTop
                Layout.maximumWidth: Style.valveSwitch.maximumWidth * 8

                columnSpacing: 5
                rowSpacing: 5
                columns: width < Style.valveSwitch.minimumWidth * 8 ? 4 : 8

                ValveSwitch { valveNumber: 1 }
                ValveSwitch { valveNumber: 2 }
                ValveSwitch { valveNumber: 3 }
                ValveSwitch { valveNumber: 4 }
                ValveSwitch { valveNumber: 5 }
                ValveSwitch { valveNumber: 6 }
                ValveSwitch { valveNumber: 7 }
                ValveSwitch { valveNumber: 8 }
            }

            Column {
                Layout.alignment: Qt.AlignTop
                Layout.maximumWidth: controlLayerValveGrid.width
                Layout.fillWidth: true

                Label {
                    text: qsTr("Pressure control")
                    font.pointSize: Style.heading1.fontSize
                    padding: Style.heading1.padding
                    leftPadding: Style.heading1.paddingLeft
                    anchors.left: parent.left
                }

                GridLayout {
                    id: pressureControlLayout
                    columns: controlLayerValveGrid.columns == 4 ? 2 : 3
                    anchors.left: parent.left
                    anchors.right: parent.right
                    rowSpacing: 20


//                    Column {
//                        Layout.fillWidth: true
//                        Layout.alignment: Qt.AlignTop
                        
//                        Label {
//                            text: qsTr("Control layer")
//                            font.pointSize: Style.heading2.fontSize
//                        }

//                        PressureController {
//                            controllerNumber: 2
//                            minPressure: 0
//                            maxPressure: 14
//                        }
//                    }

                    Column {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop

                        Label {
                            text: qsTr("Flow layer")
                            font.pointSize: Style.heading2.fontSize
                        }

                        PressureController {
                            controllerNumber: 1
                            minPressure: 0
                            maxPressure: 1
                        }
                    }

                     Column {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop
                        visible: false

                        Label {
                            text: qsTr("Pumps")
                            font.pointSize: Style.heading2.fontSize
                            bottomPadding: 10
                        }

                        PumpSwitch { pumpNumber: 1 }
                        PumpSwitch { pumpNumber: 2 }
                    }  
                }


                Label {
                    text: qsTr("Status")
                    font.pointSize: Style.heading1.fontSize
                    padding: Style.heading1.padding
                    leftPadding: Style.heading1.paddingLeft
                }

                Label {
                    text: "Microcontroller is " + Backend.connectionStatus
                    leftPadding: Style.heading1.paddingLeft
                }

                Button {
                    id: reconnectButton
                    text: qsTr("Reconnect")
                    onClicked: Backend.connect()
                    visible: Backend.connectionStatus == "Disconnected"
                }

                Button {
                    id: refreshButton
                    text: qsTr("Refresh component statuses")
                    onClicked: Backend.requestRefresh()
                    visible: Backend.connectionStatus == "Connected"
                }
            }
        }
    }
}

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

ApplicationWindow {
    id: mainWindow
    visible: true
    width: Backend.windowWidth
    height: Backend.windowHeight
    title: qsTr("Microfluidics control system")

    // if necessary, set this to be the max of the minimum widths of each view
    minimumWidth: 300
    minimumHeight: 500

    Material.theme: Backend.darkMode ? Material.Dark : Material.Light

    onWidthChanged: Backend.windowWidth = width
    onHeightChanged: Backend.windowHeight = height

    Shortcut {
        sequence: "Ctrl+W"
        onActivated: Qt.quit()
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        ManualControl{
            id: manualControlView
        }

        RoutineControl {
            id: routineControlView
        }

        LogScreen {
            id: logScreenView
        }

        Settings {
            id: settingsView
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: qsTr("Manual Experiment")
        }

        TabButton {
            text: qsTr("Auto Experiment")
        }

        TabButton {
            text: qsTr("Log")
        }

        TabButton {
            width: 50
            icon.name: "settings"
            icon.source: Backend.darkMode ? "qrc:/res/images/settings_icon_dark_theme.png" : "qrc:/res/images/settings_icon_light_theme.png"
        }
    }

    Component {
        id: graphicalControlTab
        TabButton { text: "Graphical experiment" }
    }

    Component {
        id: gcLoaderComponent
        Loader { source: Backend.currentGraphicalControlScreenURL() }
    }

    Component.onCompleted: {
        console.info("Application version: " + Backend.appVersion)

        if (Backend.graphicalControlEnabled) {
            swipeView.insertItem(1, gcLoaderComponent.createObject(swipeView));
            tabBar.insertItem(1, graphicalControlTab.createObject(tabBar))
        }
    }
}

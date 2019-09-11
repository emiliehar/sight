import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3

import guiQml 1.0
import fwVTKQml 1.0
import uiImageQml 1.0
import uiActivitiesQml 1.0
import ExActivitiesQml 1.0


Activity {
    id: exImageDisplaying
    appManager: ImageDisplayingManager {
        id: appManager
        frameBuffer: scene3D
    }

    ColumnLayout {

        anchors.fill: parent

        Rectangle {
            id: sceneRec
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#006699"

            FrameBufferItem {
                id: scene3D
                anchors.fill: parent

                onReady: appManager.createVtkScene()
            }
        }

        RowLayout {
            Layout.fillHeight: true
            Layout.maximumHeight: 50

            ComboBox {
                id: sliceEditor
                anchors.verticalCenter: parent.verticalCenter
                Layout.fillHeight: true
                anchors.leftMargin: 4

                model: ["One slice", "Three slice"]

                currentIndex: 1
                onActivated: {
                    appManager.onUpdateSliceMode((index == 0) ? 1 : 3)
                }
            }

            Button {
                id: displayScanButton
                checkable: true
                checked: true
                anchors.verticalCenter: parent.verticalCenter
                Layout.fillHeight: true

                text: "Scan"
                onCheckedChanged: {
                    sliceEditor.enabled = checked
                    appManager.onShowScan(checked)
                }
            }

            SliceSelector {
                id: sliceSelector
                Layout.fillHeight: true
                Layout.fillWidth: true

                onServiceCreated: {
                    appManager.onServiceCreated(srv)
                }
            }
        }
    }

    Component.onCompleted: {
        appManager.replaceInputs(replaceMap)
        appManager.initialize()
    }
}

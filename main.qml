import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts 

ApplicationWindow {
    visible: true
    width: 700
    height: 500
    title: "AQFPMarginMinimizer"

    Rectangle {
        anchors.fill: parent

        Image{
            sourceSize.width: parent.width
            sourceSize.height: parent.height
            source: "img/background.png"
            fillMode: Image.PreserveAspectCrop
        }
    }

    Rectangle{
        id: homeInfo
        width: 350
        height: 500
        
        ColumnLayout{
            anchors.centerIn: parent
            spacing: 10

            Text {
                text: "AQFP Margin Minimizer"
                font.pixelSize: 24
                horizontalAlignment: Text.AlignHCenter
            }

            Button {
                text: "New Simulation" 
                onClicked: {
                    actions.newButtonClicked(); 
                    label.text = "Generating now";
                } 
            }

            Button {
                text: "Load Simulation" 
                onClicked: label.text = "Loading now" 
            }

            Text {
            id: label
            text: ""
            font.pixelSize: 16
            horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}